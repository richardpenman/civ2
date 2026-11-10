FROM debian:bookworm
ENV DEBIAN_FRONTEND=noninteractive
RUN dpkg --add-architecture i386 && apt-get update && apt-get install -y --no-install-recommends \
      wine wine32 wine64 xvfb xdotool x11-apps imagemagick xauth \
      gcc-mingw-w64-i686 g++-mingw-w64-i686 binutils-mingw-w64-i686 \
      gcc make python3 python3-pefile python3-capstone python3-pil python3-numpy \
      ca-certificates file procps \
      fonts-liberation2 \
    && rm -rf /var/lib/apt/lists/*
ENV WINEDEBUG=-all WINEARCH=win32 WINEPREFIX=/wine
RUN useradd -m -u 1000 dev && mkdir /wine && chown dev /wine
USER dev
WORKDIR /work
