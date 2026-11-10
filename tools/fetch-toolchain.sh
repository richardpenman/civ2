#!/bin/sh
# Download what the portable build needs into tools/toolchain/: for its
# first two steps (tools/build_port.py module, c) wasi-sdk (clang for wasm32
# with wasi-libc) and wabt (wasm2c); for the exe step unshield's and zlib's
# sources (the set-up's readers of the game's CD) and libogg's and
# libvorbis's (its encoder of the CD's music).  macOS (Apple silicon),
# Linux (x86-64, arm64), Windows (x86-64, in an MSYS2 shell).
#   fetch-toolchain.sh [windows] [sources]
# windows: also SDL2's MinGW release, to cross-compile the Windows
# executable; sources: only the sources (unshield, zlib, libogg, libvorbis;
# and SDL2's MinGW release with windows).
set -e
mingw=; wasm=1
for a in "$@"; do
    case "$a" in
        windows) mingw=1 ;;
        sources) wasm= ;;
        *) echo "usage: $0 [windows] [sources]" >&2; exit 1 ;;
    esac
done
cd "$(dirname "$0")"
mkdir -p toolchain && cd toolchain
os=$(uname -s); arch=$(uname -m)
case "$os-$arch" in
    Darwin-arm64) sdk=arm64-macos; wabt=macos-arm64 ;;
    Darwin-x86_64) sdk=x86_64-macos; wabt= ;;
    Linux-x86_64) sdk=x86_64-linux; wabt=linux-x64 ;;
    Linux-aarch64|Linux-arm64) sdk=arm64-linux; wabt=linux-arm64 ;;
    MINGW64*-x86_64|MSYS*-x86_64|CYGWIN*-x86_64) sdk=x86_64-windows; wabt=windows-x64 ;;
    *) [ -z "$wasm" ] || { echo "no prebuilt toolchain for $os $arch" >&2; exit 1; } ;;
esac
if [ -n "$wasm" ] && [ ! -d wasi-sdk ]; then
    curl -fL# --retry 5 --retry-all-errors --retry-delay 5 https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-25/wasi-sdk-25.0-$sdk.tar.gz | tar xz
    mv wasi-sdk-25.0-$sdk wasi-sdk
fi
if [ -n "$wasm" ] && [ ! -d wabt ]; then
    if [ -z "$wabt" ]; then
        echo "wabt: no prebuilt release for this system; install it (brew install wabt) and set WABT=\$(brew --prefix wabt)" >&2
        exit 1
    fi
    curl -fL# --retry 5 --retry-all-errors --retry-delay 5 https://github.com/WebAssembly/wabt/releases/download/1.0.41/wabt-1.0.41-$wabt.tar.gz | tar xz
    mv wabt-1.0.41 wabt
fi
if [ -n "$mingw" ] && [ ! -d SDL2-mingw ]; then
    curl -fL# --retry 5 --retry-all-errors --retry-delay 5 https://github.com/libsdl-org/SDL/releases/download/release-2.30.9/SDL2-devel-2.30.9-mingw.tar.gz | tar xz
    mv SDL2-2.30.9 SDL2-mingw
fi
# The set-up's readers of the game's CD (src/host/setup.c): unshield (its
# InstallShield cabinet) and zlib; and of a disc image's music
# (src/host/setup_disc.c): libogg and libvorbis (Ogg Vorbis encoding); as
# sources, built into the program by tools/build_port.py.
fetch_src() {   # url sha256 top-level-folder name
    if [ ! -d "$4" ]; then
        curl -fL# --retry 5 --retry-all-errors --retry-delay 5 "$1" -o "$4.tar.gz"
        sum=$( (sha256sum "$4.tar.gz" 2>/dev/null || shasum -a 256 "$4.tar.gz") | cut -d' ' -f1)
        if [ "$sum" != "$2" ]; then
            echo "$4.tar.gz: not the expected file (SHA-256 $sum)" >&2
            exit 1
        fi
        tar xzf "$4.tar.gz" && rm "$4.tar.gz" && mv "$3" "$4"
    fi
}
fetch_src https://github.com/twogood/unshield/archive/refs/tags/1.6.2.tar.gz \
    a937ef596ad94d16e7ed2c8553ad7be305798dcdcfd65ae60210b1e54ab51a2f unshield-1.6.2 unshield
fetch_src https://github.com/madler/zlib/releases/download/v1.3.1/zlib-1.3.1.tar.gz \
    9a93b2b7dfdac77ceba5a558a580e74667dd6fede4585b91eefb60f03b72df23 zlib-1.3.1 zlib
fetch_src https://github.com/xiph/ogg/releases/download/v1.3.5/libogg-1.3.5.tar.gz \
    0eb4b4b9420a0f51db142ba3f9c64b333f826532dc0f48c6410ae51f4799b664 libogg-1.3.5 libogg
fetch_src https://github.com/xiph/vorbis/releases/download/v1.3.7/libvorbis-1.3.7.tar.gz \
    0e982409a9c3fc82ee06e08205b1355e5c6aa4c36bca58146ef399621b0ce5ab libvorbis-1.3.7 libvorbis
# (what unshield's cmake would configure)
cat > unshield/lib/unshield_config.h <<'EOF2'
#define HAVE_INTTYPES_H 1
#define HAVE_STDBOOL_H 1
#define HAVE_STDINT_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRING_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_TYPES_H 1
#define SIZE_FORMAT "zu"
#define USE_OUR_OWN_MD5 1
EOF2
# (what libogg's configure would write: its integer types, from stdint.h)
cat > libogg/include/ogg/config_types.h <<'EOF2'
#ifndef __CONFIG_TYPES_H__
#define __CONFIG_TYPES_H__
#include <stdint.h>
typedef int16_t ogg_int16_t;
typedef uint16_t ogg_uint16_t;
typedef int32_t ogg_int32_t;
typedef uint32_t ogg_uint32_t;
typedef int64_t ogg_int64_t;
typedef uint64_t ogg_uint64_t;
#endif
EOF2
echo "toolchain ready in tools/toolchain"
