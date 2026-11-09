"""Small, shared release protocol for the two OpenCiv2 repositories (stdlib only).

Release ZIPs have a package.json with per-file SHA-256 hashes. Consumers pin the
archive hash as well. Cached archives and extractions are content-addressed;
failed downloads/extractions never become usable cache entries.
"""
import hashlib
import json
import os
from pathlib import Path, PurePosixPath
import shutil
import stat
import tempfile
import urllib.request
import zipfile

SCHEMA = 1


def digest(path):
    h = hashlib.sha256()
    with Path(path).open('rb') as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b''):
            h.update(block)
    return h.hexdigest()


def read_json(path):
    return json.loads(Path(path).read_text(encoding='utf-8'))


def write_json(path, value):
    Path(path).parent.mkdir(parents=True, exist_ok=True)
    Path(path).write_text(json.dumps(value, indent=2, sort_keys=True) + '\n', encoding='utf-8', newline='\n')


def safe_name(name):
    p = PurePosixPath(name)
    if (not name or '\\' in name or ':' in name or p.is_absolute()
            or any(x in ('', '.', '..') for x in name.split('/'))):
        raise ValueError(f'Unsafe package path: {name!r}')
    return name


def inventory(root, names):
    root = Path(root)
    files = {}
    for name in sorted(set(names)):
        safe_name(name)
        p = root / name
        if p.is_symlink() or not p.resolve().is_relative_to(root.resolve()) or not p.is_file():
            raise ValueError(f'Not a regular package file: {p}')
        files[name] = {'sha256': digest(p), 'size': p.stat().st_size}
    return files


def validate(root, kind):
    root = Path(root)
    manifest = read_json(root / 'package.json')
    if manifest.get('schema') != SCHEMA or manifest.get('kind') != kind:
        raise ValueError(f'{root}: expected {kind} package schema {SCHEMA}')
    if not isinstance(manifest.get('files'), dict) or not manifest['files']:
        raise ValueError('Empty package inventory')
    for name, entry in manifest['files'].items():
        safe_name(name)
        path = root / name
        if (path.is_symlink() or not path.resolve().is_relative_to(root.resolve())
                or not path.is_file() or path.stat().st_size != entry['size']
                or digest(path) != entry['sha256']):
            raise ValueError(f'Package file missing or changed: {path}')
    return manifest


def release(root, names, output, kind, version):
    """Write a reproducible ZIP: sorted paths, fixed timestamp and permissions."""
    root, output = Path(root), Path(output)
    manifest = {'schema': SCHEMA, 'kind': kind, 'version': version,
                'files': inventory(root, names)}
    output.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.NamedTemporaryFile(dir=output.parent, delete=False) as f:
        temp = Path(f.name)
    try:
        with zipfile.ZipFile(temp, 'w', compression=zipfile.ZIP_DEFLATED, compresslevel=6) as archive:
            for name in ['package.json', *manifest['files']]:
                info = zipfile.ZipInfo(name, date_time=(1980, 1, 1, 0, 0, 0))
                info.create_system = 3  # Fixed Unix metadata, including on Windows.
                info.compress_type = zipfile.ZIP_DEFLATED
                info.external_attr = (stat.S_IFREG | 0o644) << 16
                with archive.open(info, 'w', force_zip64=True) as out:
                    if name == 'package.json':
                        out.write((json.dumps(manifest, indent=2, sort_keys=True) + '\n').encode())
                    else:
                        with (root / name).open('rb') as source:
                            shutil.copyfileobj(source, out)
        os.replace(temp, output)
    finally:
        temp.unlink(missing_ok=True)
    checksum = digest(output)
    output.with_suffix(output.suffix + '.sha256').write_text(f'{checksum}  {output.name}\n', encoding='ascii')
    print(f'{output}  sha256={checksum}', flush=True)
    return checksum


def cache_root():
    override = os.environ.get('CIV2_DEPENDENCY_CACHE')
    if override:
        return Path(override).expanduser().resolve()
    base = Path(os.environ.get('LOCALAPPDATA') or Path.home() / 'AppData/Local') if os.name == 'nt' else Path(os.environ.get('XDG_CACHE_HOME') or Path.home() / '.cache')
    return base / 'openciv2'


def cached_release(url, sha256, kind, cache=None):
    if len(sha256 or '') != 64 or any(c not in '0123456789abcdef' for c in sha256):
        raise ValueError('A lowercase SHA-256 archive checksum is required for release downloads')
    cache = Path(cache) if cache is not None else cache_root()
    cache.mkdir(parents=True, exist_ok=True)
    archive = cache / (sha256 + '.zip')
    target = cache / sha256
    if target.exists():
        validate(target, kind)
        return target
    if archive.exists() and digest(archive) != sha256:
        raise ValueError(f'Corrupt cached archive; remove it and retry: {archive}')
    if not archive.exists():
        with tempfile.NamedTemporaryFile(dir=cache, delete=False) as stream:
            temp = Path(stream.name)
            try:
                print(f'Downloading {kind}: {url}', flush=True)
                request = urllib.request.Request(url, headers={'User-Agent': 'OpenCiv2-build/1'})
                with urllib.request.urlopen(request, timeout=60) as response:
                    shutil.copyfileobj(response, stream)
            except BaseException:
                stream.close()
                temp.unlink(missing_ok=True)
                raise
        try:
            if digest(temp) != sha256:
                raise ValueError(f'Checksum mismatch downloading {url}')
            os.replace(temp, archive)
        finally:
            temp.unlink(missing_ok=True)
    tempdir = Path(tempfile.mkdtemp(prefix='extract-', dir=cache))
    try:
        with zipfile.ZipFile(archive) as z:
            seen = set()
            for info in z.infolist():
                name = safe_name(info.orig_filename)
                if name != info.filename:
                    raise ValueError(f'Unsafe normalized package path: {name!r}')
                mode = info.external_attr >> 16
                if name in seen or (stat.S_IFMT(mode) not in (0, stat.S_IFREG)):
                    raise ValueError(f'Unsupported or duplicate package member: {name}')
                seen.add(name)
                p = tempdir / name
                p.parent.mkdir(parents=True, exist_ok=True)
                with z.open(info) as source, p.open('wb') as out:
                    shutil.copyfileobj(source, out)
        manifest = validate(tempdir, kind)
        if seen != set(manifest['files']) | {'package.json'}:
            raise ValueError('Archive contains files outside its manifest')
        try:
            tempdir.rename(target)
        except OSError:
            if not target.is_dir():
                raise
            validate(target, kind)  # Another builder published the same package.
        return target
    finally:
        if tempdir.exists():
            shutil.rmtree(tempdir)


def resolve(root, name, local=None, url=None, sha256=None):
    """Explicit CLI > environment > sibling clone > pinned release.

    Explicit release inputs bypass sibling discovery. Invalid local paths never
    silently fall back to a different dependency or a network download.
    """
    root = Path(root)
    env = 'CIV2_' + name.upper()
    if local and url:
        raise ValueError('Select a local dependency OR a release URL')
    if local is None and url is None:
        local = os.environ.get(env + '_DIR')
        if local is None:
            url = os.environ.get(env + '_URL')
            sha256 = sha256 or os.environ.get(env + '_SHA256')
        if local is None and url is None:
            sibling = root.parent / ('openciv2-' + name)
            if sibling.is_dir():
                local = sibling
    if local is not None:
        path = Path(local).expanduser().resolve()
        if not path.is_dir():
            raise ValueError(f'Local {name} dependency not found: {path}')
        return path
    if url is None:
        locked = read_json(root / 'dependencies.json')[name]
        url, sha256 = locked['url'], locked['sha256']
    return cached_release(url, sha256, 'openciv2-' + name)


def copy_changed(source, target):
    source, target = Path(source), Path(target)
    if target.exists() and digest(source) == digest(target):
        return
    target.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.NamedTemporaryFile(dir=target.parent, delete=False) as f:
        temp = Path(f.name)
        with source.open('rb') as stream:
            shutil.copyfileobj(stream, f)
    os.replace(temp, target)
