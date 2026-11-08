# OpenCiv2 asset dependencies

The build uses two repositories, with no Git submodules:

| Repository | Owns | Release |
| --- | --- | --- |
| `openciv2-assets` | Artwork, masters, editable vectors, authoring records, review tools, selected runtime inputs, font data, pack builder and layout contracts | Runtime asset ZIP |
| `openciv2` | Game/renderer source, tests and platform packaging | Executables and portable source packages |

The graphics repository contains the full authoring library. Game builds need
only its runtime release, or a local graphics clone when editing assets.

## Local builds

Python 3.10+ is required. Install Pillow only when building the graphics clone:

```sh
python -m pip install -r openciv2-assets/requirements.txt
python openciv2-assets/tools/build.py
python openciv2-assets/tools/build.py --release --version 2026.11.07.1

python openciv2/tools/build_port.py --assets /path/to/openciv2-assets module c exe
# Cross-compile a Windows executable: replace exe with exe-windows.
```

The asset build reads selected files directly from its own `assets/atlas-hd`
directory. It has no upstream art dependency, download or import step. Retained
generated PNGs are build inputs; the runtime build does not regenerate paintings,
run authoring/review scripts, or require an original-game installation.

The game discovers a sibling `openciv2-assets` clone automatically and builds its
runtime package as needed. An explicit `--assets` path or `CIV2_ASSETS_DIR` can
instead name a clone or an extracted runtime release. Explicit paths fail if
invalid; they never silently fall back to a download.

```powershell
$env:CIV2_ASSETS_DIR = 'C:\Projects\openciv2-assets'
python C:\Projects\openciv2\tools\build_port.py atlas
```

Asset builds hash selected inputs, builder code and the Pillow version. Changes
to unselected masters or review images do not invalidate the runtime pack.
`--force` rebuilds it. A generated `build/include/hd_layouts.h` accompanies the
pack; the game installs it at `build/native/gen/hd_layouts.h` and tracks it as a
compiler dependency. `contracts/cityview-rects.json` keeps asset layout data
independent of the game checkout. The game checks that contract against its
current source before accepting a package.

## Runtime selection and release contents

`runtime-inputs.json` explicitly selects the artwork consumed by the pack
builder. All image and metadata reads must be selected, and image-bank scans
enumerate that selection. Adding a neighboring image to the source tree does
not add it to the game. Review and authoring directories cannot be selected as
runtime inputs. Update the selection and pack logic together when adding assets.

The release ZIP contains exactly these files, plus its `package.json` inventory:

- `data/atlas-hd.pack`
- `data/fonts.bin` and `data/fonts_aa.bin`
- `data/ATLAS-PROVENANCE.md`
- `data/cityview-rects.json`
- `include/hd_layouts.h`
- `LICENSE` and `THIRD-PARTY.txt`

Masters, SVG sources, individual PNGs, authoring fonts, generation records,
comparison images, review tools, and build diagnostics stay in the repository
or local build tree. Original-game comparisons have the scope described in
the artwork provenance and are excluded from the runtime pack and release.
The game carries the asset notice as `ASSET-NOTICES.txt` alongside its own
software notices. Portable source packages retain it under `build/data`.

## Release downloads and caching

With no explicit dependency or sibling clone, game builds download the URL
and SHA-256 pinned in the game's `dependencies.json`. The asset repository has
no dependency lock file. The runtime release tag is `v2026.11.07.1`; its archive
must be published before those default download URLs can be used.

```sh
python openciv2/tools/build_port.py --assets-url URL --assets-sha256 SHA256 atlas
```

URLs can be HTTPS release assets or `file:///...` archives. An explicit URL
requires a checksum and bypasses sibling discovery. Selection precedence is:
explicit CLI options, environment overrides, sibling clone, then pinned release.
The environment variables are `CIV2_ASSETS_DIR`, `CIV2_ASSETS_URL` and
`CIV2_ASSETS_SHA256`; the directory override takes precedence over the URL.

The cache defaults to `%LOCALAPPDATA%/openciv2` on Windows and
`${XDG_CACHE_HOME:-~/.cache}/openciv2` elsewhere. Override it with
`CIV2_DEPENDENCY_CACHE`. ZIPs and extractions are keyed by archive SHA-256;
validated cached packages work offline. The resolver verifies archive and
per-file hashes, rejects unsafe ZIP paths and publishes only complete
extractions. Remove a corrupt cache entry to redownload. Runtime manifests use
schema 1 and kind `openciv2-assets`.

`CIV2_ATLAS_PREBUILT=1` is reserved for prepared portable-C packages and CI
artifacts. It verifies `build/data/assets-receipt.json`, including the header
and notices. Portable-C packages also recognize that receipt automatically.
Normal clone builds should leave the setting unset. Consuming a runtime release
needs neither Pillow nor the graphics source checkout.

## Updating and publishing

1. Edit artwork and packing changes together in `openciv2-assets`. Keep
   `runtime-inputs.json` synchronized with the approved runtime selection.
2. Run `python -m unittest discover -s tests`, build the pack, and run
   `python tools/test_hd_factions.py` in the graphics repository. Review the
   rendered result in the game.
3. Run `python tools/build.py --release --version VERSION`. The ZIP and its
   `.sha256` sidecar are written to `dist`. Use the pinned Pillow version in
   `requirements.txt`; ZIP timestamps, order and permissions are fixed.
4. Publish the runtime ZIP, then update the URL/checksum in the game's
   `dependencies.json` and build/test the game. Never replace a published pinned
   release with different bytes; use a new version/tag.

The graphics workflow builds and publishes runtime assets for `v*` tags.
Its manual dispatch builds artifacts without publishing. No separate art
release is needed. `make`, `make windows`, direct `build_native.py` builds and
the game release workflow all prepare the runtime dependency.

The intended remotes are `PeterJohnson/openciv2-assets` and
`PeterJohnson/openciv2`. Game development is on `codex/hd-graphics-runtime`;
its local `main` remains at the upstream base. Publish the runtime
archive before pushing game changes that require it in CI. Existing remote
graphics branches and their large history are unaffected by this local setup.

The stdlib-only package helpers and resolver tests live in both repositories;
keep those copies synchronized when changing the release protocol.
