# Civilization II reconstruction — common entry points.  Build and play
# locally with make / make run; development targets use Docker (tools/dk).
# See README.md.  (src/auto is edited directly: the
# Ghidra regeneration is retired; tools/gen_c.py refuses without
# CIV2_REGENERATE=1.  The Windows graft build is retired: tag pre-step5.)

DK      = tools/dk
SEED   ?= 7
SCRIPT ?= harness/scripts/newgame.txt
NAME   := $(basename $(notdir $(SCRIPT)))

.PHONY: all build run windows images hook ref native port native-suite clean

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

hook:                         ## proxy XDaemon.dll: seed control, rand trace, replay, crash log
	$(DK) make -C hook






ref:                          ## record the original game on SCRIPT (deterministic replay)
	$(DK) python3 harness/run.py --build orig --seed $(SEED) --script $(SCRIPT) --out run/ref-$(NAME)




clean:
	rm -rf build run
