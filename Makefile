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

.PHONY: all build run windows web serve deploy-review web-test web-replay hd-art images hook ref native port native-suite test test-sound test-extras extras-bless test-media media-bless unit-record clean

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

build:                        ## build/OpenCiv2 (first time: downloads its toolchain into tools/toolchain)
	python3 tools/build_port.py

run: | build/OpenCiv2$(EXE)   ## play the game in GAME (default $$CIV2_GAME or ~/Civ2; Windows: %USERPROFILE%\Civ2); builds first if there is no build
	$(PLAY)

build/OpenCiv2$(EXE):
	python3 tools/build_port.py

windows:                      ## build/OpenCiv2.exe, cross-compiled from Linux
	python3 tools/build_port.py module c exe-windows

# The browser version (README "Browser version", docs/DEVELOPMENT.md): a
# static site in build/web; its tests in Chrome's headless shell (Linux
# x86-64; they need the game files, $$CIV2_GAME or ~/Civ2).
web:                          ## build/web: the browser version (first time: downloads Binaryen into tools/toolchain; GAME=1: with the installed game, for private use only)
	GAME=$(GAME) python3 tools/build_web.py

serve: web                    ## build/web served at http://localhost:$(PORT) (PORT=8000); Ctrl+C stops it
	@echo "OpenCiv2: http://localhost:$(PORT)"
	python3 -m http.server -d build/web $(PORT)

deploy-review:                ## the browser version with the game (GAME=1: for private use only) served on this machine by Caddy in Docker at /civ2/, behind a password (tools/deploy_review.sh; ~/civ2-review/config)
	tools/deploy_review.sh

web-test: web                 ## the browser version's tests: its host's logic, the set-up from a made-up disc image, then the smoke test (menu, new game, save, saves exported and imported, load; run/web-smoke), then HD graphics (run/web-hd)
	sh tools/fetch-toolchain.sh browser
	tools/toolchain/node/bin/node src/web/test/unit.mjs
	tools/toolchain/node/bin/node src/web/test/setup.mjs
	tools/toolchain/node/bin/node src/web/test/net.mjs
	tools/toolchain/node/bin/node src/web/test/smoke.mjs
	tools/toolchain/node/bin/node src/web/test/hd.mjs

web-replay: web               ## SCRIPT replayed in the browser version, compared with the original's recording
	sh tools/fetch-toolchain.sh browser
	tools/toolchain/node/bin/node src/web/test/smoke.mjs --replay $(NAME)
	$(DK) sh -c "python3 -c \"import glob,sys; from PIL import Image; [Image.open(f).save(f[:-4] + '.png') for f in glob.glob('run/web-$(NAME)/*.bmp')]\" && python3 harness/compare.py run/ref-$(NAME) run/web-$(NAME)"

# HD graphics (docs/HD-GRAPHICS.md): data/native/hd made again from the
# release of Peter Johnson's art pinned in data/hd-pack.json (Docker).
hd-art:                       ## make the HD graphics' art in data/native/hd again from the pinned release (tools/build_hd_pack.py)
	python3 tools/build_hd_pack.py

images:                       ## build the Docker images (wine/mingw/python/SDL2/wasi-sdk; Ghidra)
	docker build -t civ2-dev-base tools/docker
	docker build -t civ2-dev tools/docker-ext
	docker build -t civ2-ghidra tools/ghidra-docker

native:                       ## build/civ2-linux32: the native SDL2 build (32-bit Linux, in Docker)
	$(DK) python3 tools/build_native.py --target linux32

port:                         ## build/OpenCiv2 in Docker (`make build` builds it on this machine)
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
	$(DK) sh -c "cc -O2 -DHAVE_CONFIG_H -Isrc/third_party/libwebp -o build/hd_image_test tests/hd_image_test.c src/platform/hd_image.c src/third_party/libwebp/src/dec/*.c src/third_party/libwebp/src/dsp/*.c src/third_party/libwebp/src/utils/*.c -lm && build/hd_image_test"
	$(DK) sh -c "for t in tests/*_test.cpp; do CXX='g++ -m32' python3 tests/source_probe.py \$$t >/dev/null 2>&1 && echo \$$t passed || { echo \$$t FAILED; exit 1; }; done"
	$(DK) python3 tests/lang_test.py
	$(DK) python3 tests/text_test.py /civ2
	$(DK) python3 tests/text_keys_test.py /civ2

# The additions' regression tests (harness/extras_suite.py): our own output,
# checked by eye and recorded (extras-bless), replayed and compared.  Needs
# the game files; NAMES limits them to some scripts.
test-sound: native           ## OpenCiv2's own sound effects: without the game's Sound folder the start menu's is made from data/native/sound and played (tests/sound_fallback_test.sh)
	$(DK) sh tests/sound_fallback_test.sh

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




clean:
	rm -rf build run
