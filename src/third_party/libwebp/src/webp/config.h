/* OpenCiv2: libwebp's configuration (HAVE_CONFIG_H, tools/build_native.py):
   no WEBP_HAVE_* instruction sets, so the plain C everywhere (the decoder is
   compiled to wasm for every build but the linux32 development ones), and no
   threads (WEBP_USE_THREAD unset). */
#ifndef WEBP_WEBP_CONFIG_H_
#define WEBP_WEBP_CONFIG_H_
#endif
