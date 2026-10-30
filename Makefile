# Civilization II reconstruction — common entry points.  Everything runs in
# Docker (tools/dk); see README.md.  (src/auto is edited directly: the
# Ghidra regeneration is retired; tools/gen_c.py refuses without
# CIV2_REGENERATE=1.  The Windows graft build is retired: tag pre-step5.)

DK      = tools/dk
SEED   ?= 7
SCRIPT ?= harness/scripts/newgame.txt
NAME   := $(basename $(notdir $(SCRIPT)))

.PHONY: all build run windows images hook ref native port native-suite test test-extras extras-bless unit-record clean

# Build and play on this machine (no Docker): needs a C compiler, SDL2 and
# Python 3 (README "Build and run").  On Windows: in an MSYS2 MINGW64 shell.
ifeq ($(OS),Windows_NT)
EXE  := .exe
PLAY  = cmd //c "tools\run-civ2.bat $(GAME)"
else
PLAY  = tools/run-civ2.sh $(GAME)
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
                menuview-arrange reports screens setup spriteedit

test:                         ## the unit tests: the recorded cases in tests/unit (build/civ2-unittest), then the source probes (tests/*_test.cpp)
	$(DK) sh -c "python3 tools/build_native.py --target linux32test >/dev/null && cd build && CIV2_DATA=../data/native ./civ2-unittest --mask ../tests/unit/textmask.tsv ../tests/unit/*.gz"
	$(DK) sh -c "for t in tests/*_test.cpp; do CXX='g++ -m32' python3 tests/source_probe.py \$$t >/dev/null 2>&1 && echo \$$t passed || { echo \$$t FAILED; exit 1; }; done"

# The additions' regression tests (harness/extras_suite.py): our own output,
# checked by eye and recorded (extras-bless), replayed and compared.  Needs
# the game files; NAMES limits them to some scripts.
test-extras: native          ## replay harness/extras on build/civ2-linux32 and compare with harness/extras/expected
	$(DK) python3 harness/extras_suite.py $(NAMES)

extras-bless: native         ## record harness/extras/expected again (check run/extras-*/*.png by eye first)
	$(DK) python3 harness/extras_suite.py --bless $(NAMES)

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
