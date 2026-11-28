# The CI's image (.github/workflows/ci.yml): the dev image's (tools/docker,
# tools/docker-ext: civ2-dev) Debian 12 and its gcc 12 (the unit tests'
# cases and the extras' expected results were recorded with them), with
# what the native builds and their tests need, and not Wine, MinGW, gdb or
# Xvfb (the original game is never run in CI).  Keep the packages in step
# with those two.  tools/dk runs it with CIV2_IMAGE=civ2-ci:
#   docker build --build-arg UID=$(id -u) -t civ2-ci tools/docker-ci
# (UID: the user's, so that what it writes in the checkout is the user's)
FROM debian:bookworm
ENV DEBIAN_FRONTEND=noninteractive
RUN dpkg --add-architecture i386 && apt-get update && apt-get install -y --no-install-recommends \
      gcc g++ make python3 python3-pil python3-numpy gcc-multilib g++-multilib \
      libsdl2-dev libsdl2-dev:i386 ca-certificates curl file procps fonts-liberation2 \
    && rm -rf /var/lib/apt/lists/* /usr/share/doc
RUN curl -fsSL https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-25/wasi-sdk-25.0-x86_64-linux.tar.gz \
        | tar xz -C /opt && ln -s /opt/wasi-sdk-25.0-x86_64-linux /opt/wasi-sdk \
    && curl -fsSL https://github.com/WebAssembly/wabt/releases/download/1.0.41/wabt-1.0.41-linux-x64.tar.gz \
        | tar xz -C /opt && ln -s /opt/wabt-1.0.41 /opt/wabt
ENV PATH="/opt/wabt/bin:${PATH}"
ARG UID=1000
RUN useradd -m -u $UID dev
USER dev
WORKDIR /work
