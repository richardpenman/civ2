/* tests/music_fallback_test.sh: each Ogg Vorbis file named decoded whole by
 * stb_vorbis, as the game's music player decodes it (src/host/host_sdl.c),
 * and a line "FILE CHANNELS RATE SECONDS" printed; exit 1 if one is not.
 *   cc -O2 -Isrc/third_party/stb_vorbis -o music_decode tests/music_decode.c -lm */
#include "stb_vorbis.c"

int main(int argc, char **argv)
{
    int i, bad = 0;
    for (i = 1; i < argc; i++) {
        int chans = 0, rate = 0;
        short *pcm = NULL;
        int frames = stb_vorbis_decode_filename(argv[i], &chans, &rate, &pcm);
        if (frames <= 0 || rate <= 0) {
            printf("%s not decoded\n", argv[i]);
            bad = 1;
        } else
            printf("%s %d %d %.2f\n", argv[i], chans, rate, (double)frames / rate);
        free(pcm);
    }
    return bad;
}
