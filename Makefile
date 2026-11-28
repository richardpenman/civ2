# Civilization II reconstruction — common entry points.  Build and play
# locally with make / make run; development targets use Docker (tools/dk).
# See README.md.  (src/auto is edited directly: the
# Ghidra regeneration is retired; tools/gen_c.py refuses without
# CIV2_REGENERATE=1.  The Windows graft build is retired: tag pre-step5.)

DK      = tools/dk
SEED   ?= 7
SCRIPT ?= harness/scripts/newgame.txt
NAME   := $(basename $(notdir $(SCRIPT)))

PORT ?= 8000

# The HD graphics' art (docs/HD-GRAPHICS.md), data/native/hd: not in the
# repository, made from Peter Johnson's art at the commit of openciv2-assets
# pinned in data/hd-pack.json (tools/build_hd_pack.py: the files it is built
# from fetched once into ~/.cache/openciv2, the pack built and the pictures
# encoded in Docker or by this machine's Python and cwebp, the result kept
# there too) before each build that packs or runs with it; at once when
# it is up to date.  HD_ART= (or 0): builds without it (the game then has
# Classic graphics only).
HD_ART ?= 1
HD_ART_MAKE = $(if $(filter-out 0,$(HD_ART)),python3 tools/build_hd_pack.py,@echo "(no HD art: HD_ART=$(HD_ART))")

# OpenCiv2's own assets, data/native/music (docs/NO-INSTALL.md, Music),
# data/native/sound (docs/NO-INSTALL.md, Sound effects),
# data/native/heralds (docs/HD-GRAPHICS.md, Your own heralds) and
# data/native/wonders (docs/NO-INSTALL.md, Wonder movies): not in the
# repository, made from OpenCiv2's repository of assets
# openciv2/openciv2-assets (its assets/music, sound, heralds, wonders; the HD
# art is its assets/atlas-hd) at the commit pinned in data/own-assets.json
# (tools/build_own_assets.py: the files, the music and sound effects already
# Ogg Vorbis as the game plays them, fetched once into ~/.cache/openciv2 and
# copied as they are, nothing converted: Python 3 only) before each build
# that packs or runs with them (make, make run's first build, make port,
# native, windows, web); at once when they are up to date.  OWN_ASSETS= (or
# 0): builds without them (the game then has music and sound effects only
# from its own folder, and no heralds of OpenCiv2's but
# data/native/hd-extra's).
OWN_ASSETS ?= 1
OWN_ASSETS_MAKE = $(if $(filter-out 0,$(OWN_ASSETS)),python3 tools/build_own_assets.py,@echo "(no assets of OpenCiv2's own: OWN_ASSETS=$(OWN_ASSETS))")

.PHONY: all build run windows web serve deploy-review web-test web-parity web-replay hd-art own-assets images hook ref native port native-suite test test-sound test-music test-wonder test-extras extras-bless test-media media-bless unit-record clean clean-run

# Build and play on this machine (no Docker): needs a C compiler, SDL2 and
# Python 3 (README "Build and run").  On Windows: in an MSYS2 UCRT64 shell,
# with MSYS2's python.
ifeq ($(OS),Windows_NT)
EXE  := .exe
PLAY  = cmd //c "tools\run-civ2.bat $(GAME)"
else
PLAY  = tools/run-civ2.sh "$(GAME)"
endif

all: build

build:                        ## build/OpenCiv2, the HD art and OpenCiv2's own music, sound effects, heralds and wonder movies (first time: downloads its toolchain into tools/toolchain and the art's and OpenCiv2's own assets' sources into ~/.cache/openciv2)
	$(HD_ART_MAKE)
	$(OWN_ASSETS_MAKE)
	python3 tools/build_port.py

run: | build/OpenCiv2$(EXE)   ## play the game in GAME (default $$CIV2_GAME or ~/Civ2; Windows: %USERPROFILE%\Civ2); builds first if there is no build
	$(PLAY)

build/OpenCiv2$(EXE):
	$(HD_ART_MAKE)
	$(OWN_ASSETS_MAKE)
	python3 tools/build_port.py

windows:                      ## build/OpenCiv2.exe, cross-compiled from Linux
	$(HD_ART_MAKE)
	$(OWN_ASSETS_MAKE)
	python3 tools/build_port.py module c exe-windows

# The browser version (README "Browser version", docs/DEVELOPMENT.md): a
# static site in build/web; its tests in Chrome's headless shell (Linux
# x86-64; they need the game files, $$CIV2_GAME or ~/Civ2).
web:                          ## build/web: the browser version (first time: downloads Binaryen into tools/toolchain; GAME=1: with the installed game, for private use only)
	$(HD_ART_MAKE)
	$(OWN_ASSETS_MAKE)
	HD_ART=$(HD_ART) OWN_ASSETS=$(OWN_ASSETS) GAME=$(GAME) python3 tools/build_web.py

serve: web                    ## build/web served at http://localhost:$(PORT) (PORT=8000); Ctrl+C stops it
	@echo "OpenCiv2: http://localhost:$(PORT)"
	python3 -m http.server -d build/web $(PORT)

deploy-review:                ## the browser version with the game (GAME=1: for private use only) and the HD art, served on this machine by Caddy in Docker at /civ2/, behind a password (tools/deploy_review.sh; ~/civ2-review/config)
	tools/deploy_review.sh

web-test: web                 ## the browser version's tests: its host's logic, the set-up from a made-up disc image, then the start without the original game (fresh storage: the start screen, then HD graphics and OpenCiv2's own files to the main menu; run/web-start), a unit's moves timed in a new game (run/web-perf), then the smoke test (menu, new game, save, saves exported and imported from the start screen, load; run/web-smoke), then HD graphics (run/web-hd), then no looping sound left after the menus or the diplomacy room (run/web-sound), then a phone's touch screen (layout sideways and upright, taps, long presses, drags, the page's gestures blocked, full screen; run/web-mobile)
	sh tools/fetch-toolchain.sh browser
	tools/toolchain/node/bin/node src/web/test/unit.mjs
	tools/toolchain/node/bin/node src/web/test/setup.mjs
	tools/toolchain/node/bin/node src/web/test/net.mjs
	tools/toolchain/node/bin/node src/web/test/start.mjs
	tools/toolchain/node/bin/node src/web/test/perf.mjs
	tools/toolchain/node/bin/node src/web/test/smoke.mjs
	tools/toolchain/node/bin/node src/web/test/hd.mjs
	tools/toolchain/node/bin/node src/web/test/sound.mjs
	tools/toolchain/node/bin/node src/web/test/mobile.mjs

web-parity: web               ## the desktop's replays in the browser version: a representative set of harness/extras (with and without the original's files, Classic and HD graphics), of harness/media (the CD's herald and wonder movies, Classic and HD, OpenCiv2's own Pyramids movie; calls.log too) and three native-suite games, each screenshot and the rand() trace compared with the desktop's expected results (NAMES=...: those cases; --list: all of them); mismatches in run/web-parity/CASE (src/web/test/web-parity.mjs)
	sh tools/fetch-toolchain.sh browser
	tools/toolchain/node/bin/node src/web/test/web-parity.mjs $(NAMES)

web-replay: web               ## SCRIPT replayed in the browser version, compared with the original's recording
	sh tools/fetch-toolchain.sh browser
	tools/toolchain/node/bin/node src/web/test/smoke.mjs --replay $(NAME)
	$(DK) sh -c "python3 -c \"import glob,sys; from PIL import Image; import os; [(Image.open(f).save(f[:-4] + '.png'), os.remove(f)) for f in glob.glob('run/web-$(NAME)/*.bmp')]\" && python3 harness/compare.py run/ref-$(NAME) run/web-$(NAME)"

# HD graphics (docs/HD-GRAPHICS.md): data/native/hd from Peter Johnson's
# art at the commit pinned in data/hd-pack.json (HD_ART above; FORCE=1:
# made again, whatever is there or in the cache).
hd-art:                       ## the HD graphics' art in data/native/hd from the pinned commit of openciv2/openciv2-assets (tools/build_hd_pack.py; at once when up to date)
	python3 tools/build_hd_pack.py $(if $(FORCE),--force)

# OpenCiv2's own music, sound effects, heralds and wonder movies:
# data/native/music, data/native/sound, data/native/heralds and
# data/native/wonders from the commit of
# openciv2/openciv2-assets pinned in data/own-assets.json (OWN_ASSETS above;
# fetched once, about 35 MB, and copied; FORCE=1: made again, whatever is
# there or in the cache).
own-assets:                   ## OpenCiv2's own music, sound effects, heralds and wonder movies in data/native/music, sound, heralds, wonders, fetched from the pinned commit of openciv2/openciv2-assets and copied (tools/build_own_assets.py; at once when up to date)
	python3 tools/build_own_assets.py $(if $(FORCE),--force)

images:                       ## build the Docker images (wine/mingw/python/SDL2/wasi-sdk; Ghidra)
	docker build -t civ2-dev-base tools/docker
	docker build -t civ2-dev tools/docker-ext
	docker build -t civ2-ghidra tools/ghidra-docker

native:                       ## build/civ2-linux32: the native SDL2 build (32-bit Linux, in Docker)
	$(HD_ART_MAKE)
	$(OWN_ASSETS_MAKE)
	$(DK) python3 tools/build_native.py --target linux32

port:                         ## build/OpenCiv2 in Docker (`make build` builds it on this machine)
	$(HD_ART_MAKE)
	$(OWN_ASSETS_MAKE)
	$(DK) python3 tools/build_port.py

native-suite:                 ## run every scenario on a native build (EXE=OpenCiv2 or civ2-linux32) against the Wine recordings
	$(DK) sh -c "EXE=$(or $(EXE),OpenCiv2) sh harness/native_suite.sh"

# The unit tests (docs/DEVELOPMENT.md): calls of the game's functions recorded
# from these replays, run again and compared in seconds.  No game files needed.
UNIT_SCRIPTS ?= long50 turns newgame conquest civilwar spies units talks citywork hotseat accelerated \
                napoleon ww1 actions actions2 events scenario saveload endgame advisors citywindow conquest-spy \
                editors editors2 hotseat-scenario mapview menucheat menuitems menuorders menuview \
                menuview-arrange reports screens setup spriteedit cityunits editrename pediascroll premade \
                rarecalls rome spritepick uimisc

test:                         ## the unit tests: the recorded cases in tests/unit (build/civ2-unittest), the built-in rules against the game's RULES.TXT (skipped without it), then the HD pictures' decoder (tests/hd_image_test.c), then the source probes (tests/*_test.cpp), then the translations (tests/lang_test.py), then OpenCiv2's English of the game's texts (tests/text_test.py), then that every section and block of the game's text files is keyed for translation (tests/text_keys_test.py)
	$(DK) sh -c "python3 tools/build_native.py --target linux32test >/dev/null && cd build && CIV2_DATA=../data/native ./civ2-unittest --mask ../tests/unit/textmask.tsv ../tests/unit/*.gz"
	$(DK) sh -c "cd build && CIV2_DATA=../data/native ./civ2-unittest --rules /civ2"
	$(DK) sh -c "cc -O2 -DHAVE_CONFIG_H -Isrc/third_party/libwebp -o build/hd_image_test tests/hd_image_test.c src/platform/hd_image.c src/third_party/libwebp/src/dec/*.c src/third_party/libwebp/src/demux/*.c src/third_party/libwebp/src/dsp/*.c src/third_party/libwebp/src/utils/*.c -lm && build/hd_image_test"
	$(DK) sh -c "for t in tests/*_test.cpp; do CXX='g++ -m32' python3 tests/source_probe.py \$$t >/dev/null 2>&1 && echo \$$t passed || { echo \$$t FAILED; exit 1; }; done"
	$(DK) python3 tests/lang_test.py
	$(DK) python3 tests/text_test.py /civ2
	$(DK) python3 tests/text_keys_test.py /civ2

# The additions' regression tests (harness/extras_suite.py): our own output,
# checked by eye and recorded (extras-bless), replayed and compared.  Needs
# the game files; NAMES limits them to some scripts.
test-sound: native           ## OpenCiv2's own sound effects: with Classic graphics the start menu's made from data/native/sound and played without the game's Sound folder, none made with it; with HD graphics OpenCiv2's first, with the folder or without (tests/sound_fallback_test.sh; skipped without data/native/sound: make own-assets); then no looping sound left (the menus' theme, the diplomacy room's drums) after the menus, a game loaded, the diplomacy room closed every way, the opening video, and never one loop twice at once (tests/sound_loops_test.sh, harness/sound)
	$(DK) sh -c "sh tests/sound_fallback_test.sh; r=\$$?; sh tests/sound_loops_test.sh || r=1; exit \$$r"

test-music: native            ## OpenCiv2's own music: without the game's Music folder and with audio the game plays track NN from data/native/music, decoded; with the folder the game's own (tests/music_fallback_test.sh; skipped without data/native/music)
	$(DK) sh tests/music_fallback_test.sh

test-wonder: native           ## OpenCiv2's own wonder movies: the Pyramids' (data/native/wonders) played, picture and sound, with HD graphics (over the game's Video too) and with Classic graphics where the game has no Video; the game's own WONDER00.AVI with Classic graphics where it has; none in other replays (tests/wonder_movie_test.sh; skipped without data/native/wonders)
	$(DK) sh tests/wonder_movie_test.sh

test-extras: native          ## replay harness/extras on build/civ2-linux32 and compare with harness/extras/expected
	$(DK) python3 harness/extras_suite.py $(NAMES)

extras-bless: native         ## record harness/extras/expected again (check run/extras-*/*.png by eye first)
	$(DK) python3 harness/extras_suite.py --bless $(NAMES)

# The media tests (harness/media, opt-in, not part of the suites above): the
# opening, council, heralds, wonder movies and music, with the CD's KINGS,
# Video and Music in the game folder (skipped without them).
test-media: native           ## replay harness/media (the CD's videos and music) and compare with harness/media/expected
	$(DK) python3 harness/extras_suite.py --media $(NAMES)

media-bless: native          ## record harness/media/expected again (check run/media-*/*.png by eye first)
	$(DK) python3 harness/extras_suite.py --media --bless $(NAMES)

unit-record:                  ## record the unit tests' cases again from the replays (needs the game and run/ref-*)
	$(DK) python3 tools/build_native.py --target linux32rec
	$(DK) python3 tools/build_native.py --target linux32test
	$(DK) sh -c "python3 tools/unit_textmask.py build/civ2-unittest > tests/unit/textmask.tsv"
	$(DK) sh tools/unit_record.sh $(patsubst %,harness/scripts/%.txt,$(UNIT_SCRIPTS))
	$(DK) python3 tools/unit_coverage.py

hook:                         ## proxy XDaemon.dll: seed control, rand trace, replay, crash log
	$(DK) make -C hook






ref:                          ## record the original game on SCRIPT (deterministic replay)
	$(DK) python3 harness/run.py --build orig --seed $(SEED) --script $(SCRIPT) --out run/ref-$(NAME)




clean-run:                    ## remove run/ outputs unchanged for DAYS days (default 3; DRY=1: only say what); keeps .keep, ref-*
	python3 tools/clean_run.py --days $(or $(DAYS),3) $(if $(filter-out 0,$(DRY)),--dry-run)

clean:
	rm -rf build run

# The whole local test suite with the original game installed, before every
# push (docs/DEVELOPMENT.md, Before pushing): what CI cannot run without the
# install too.  Serial, keeps going after a failure; logs and a summary in
# run/check-all (tools/check_all.sh; ONLY="step ...": only those steps).
.PHONY: check-all
check-all:                    ## before every push: everything, with the install (make test, the builds, test-sound/music/wonder/extras/media, hd-portraits.sh, the browser's tests, web-parity, native-suite EXE=OpenCiv2, diff_suite.sh), serially, a summary at the end (about 75 minutes; needs the install and Docker: sg docker -c "make check-all")
	tools/check_all.sh
