"""Where things are.  The original game is not part of this repository: its
directory is $CIV2_GAME, by default ~/Civ2 (in the dev container: /civ2,
mounted there by tools/dk)."""
import os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GAME = os.path.expanduser(os.environ.get('CIV2_GAME') or '~/Civ2')


def game_file(name):
    """A file of the installed game, e.g. game_file('civ2.exe')."""
    path = os.path.join(GAME, name)
    if not os.path.exists(path):
        # the game's files are upper and lower case in different releases
        for f in os.listdir(GAME) if os.path.isdir(GAME) else ():
            if f.lower() == name.lower():
                return os.path.join(GAME, f)
        raise SystemExit('%s not found: install Civilization II in ~/Civ2 or set CIV2_GAME' % path)
    return path


def _first_dir(*candidates):
    for d in candidates:
        if d and os.path.isdir(d):
            return d
    return candidates[-1]


# The portable build's toolchain: $WASI_SDK / $WABT, else what
# tools/fetch-toolchain.sh downloaded, else the dev container's /opt.
WASI_SDK = _first_dir(os.environ.get('WASI_SDK'), os.path.join(ROOT, 'tools', 'toolchain', 'wasi-sdk'), '/opt/wasi-sdk')
WABT = _first_dir(os.environ.get('WABT'), os.path.join(ROOT, 'tools', 'toolchain', 'wabt'), '/opt/wabt')
# SDL2's MinGW release, for cross-compiling the Windows executable.
SDL2_MINGW = _first_dir(os.environ.get('SDL2_MINGW'), os.path.join(ROOT, 'tools', 'toolchain', 'SDL2-mingw'))
# unshield's and zlib's sources, for the set-up from the game's CD
# (src/host/setup.c; tools/fetch-toolchain.sh).
UNSHIELD = os.environ.get('UNSHIELD_SRC') or os.path.join(ROOT, 'tools', 'toolchain', 'unshield')
ZLIB = os.environ.get('ZLIB_SRC') or os.path.join(ROOT, 'tools', 'toolchain', 'zlib')
# libogg's and libvorbis's sources, for the music from a disc image
# (src/host/setup_disc.c; tools/fetch-toolchain.sh).
OGG = os.environ.get('OGG_SRC') or os.path.join(ROOT, 'tools', 'toolchain', 'libogg')
VORBIS = os.environ.get('VORBIS_SRC') or os.path.join(ROOT, 'tools', 'toolchain', 'libvorbis')


def hd_pack_defines():
    """The HD graphics' pinned pack (data/hd-pack.json) for the host's download
    of it (src/host/host_launcher.c): -D flags for the C compiler."""
    import json
    pin = json.load(open(os.path.join(ROOT, 'data', 'hd-pack.json')))
    return ['-DHD_PACK_TAG="%s"' % pin['tag'], '-DHD_PACK_URL="%s"' % pin['url'],
            '-DHD_PACK_SHA256="%s"' % pin['sha256'], '-DHD_PACK_SIZE=%dL' % pin['size']]
