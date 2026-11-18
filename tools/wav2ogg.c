/* Raw PCM (16-bit little-endian stereo, 44.1 kHz: a CD's audio, a music
 * track's .wav data) on stdin as Ogg Vorbis on stdout, at the quality the
 * set-up encodes the CD's music with (src/host/setup_disc.c: MUSIC_QUALITY).
 * tools/build_web.py builds it (cc, libogg and libvorbis of tools/toolchain)
 * for a site with the game (make web GAME=1): Music/TrackNN.ogg.
 *
 *   wav2ogg TRACK_NUMBER < data.pcm > TrackNN.ogg
 */
#include <stdio.h>
#include <stdlib.h>
#include <vorbis/vorbisenc.h>

#define MUSIC_QUALITY 0.4f

static void page_out(const ogg_page *og)
{
    fwrite(og->header, 1, (size_t)og->header_len, stdout);
    fwrite(og->body, 1, (size_t)og->body_len, stdout);
}

int main(int argc, char **argv)
{
    static unsigned char raw[4096 * 4];
    vorbis_info vi;
    vorbis_comment vc;
    vorbis_dsp_state vd;
    vorbis_block vb;
    ogg_stream_state os;
    ogg_page og;
    ogg_packet op, comm, code;
    int eos = 0;
    vorbis_info_init(&vi);
    if (vorbis_encode_init_vbr(&vi, 2, 44100, MUSIC_QUALITY))
        return 1;
    vorbis_comment_init(&vc);
    vorbis_comment_add_tag(&vc, "ENCODER", "OpenCiv2 wav2ogg");
    vorbis_analysis_init(&vd, &vi);
    vorbis_block_init(&vd, &vb);
    ogg_stream_init(&os, argc > 1 ? atoi(argv[1]) : 1);
    vorbis_analysis_headerout(&vd, &vc, &op, &comm, &code);
    ogg_stream_packetin(&os, &op);
    ogg_stream_packetin(&os, &comm);
    ogg_stream_packetin(&os, &code);
    while (ogg_stream_flush(&os, &og))
        page_out(&og);
    while (!eos) {
        long frames = (long)(fread(raw, 1, sizeof raw, stdin) / 4), i;
        if (frames > 0) {
            float **buf = vorbis_analysis_buffer(&vd, (int)frames);
            for (i = 0; i < frames; i++) {
                const unsigned char *s = raw + i * 4;
                buf[0][i] = (short)(s[0] | s[1] << 8) / 32768.f;
                buf[1][i] = (short)(s[2] | s[3] << 8) / 32768.f;
            }
        }
        vorbis_analysis_wrote(&vd, (int)frames);       /* (0: the end) */
        while (vorbis_analysis_blockout(&vd, &vb) == 1) {
            vorbis_analysis(&vb, NULL);
            vorbis_bitrate_addblock(&vb);
            while (vorbis_bitrate_flushpacket(&vd, &op)) {
                ogg_stream_packetin(&os, &op);
                while (ogg_stream_pageout(&os, &og)) {
                    page_out(&og);
                    if (ogg_page_eos(&og))
                        eos = 1;
                }
            }
        }
    }
    ogg_stream_clear(&os);
    vorbis_block_clear(&vb);
    vorbis_dsp_clear(&vd);
    vorbis_comment_clear(&vc);
    vorbis_info_clear(&vi);
    return fflush(stdout) != 0;
}
