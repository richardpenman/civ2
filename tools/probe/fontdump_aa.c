/*
 * Capture the antialiased form of the game's fonts as the reference
 * environment (Wine) uses it on 32-bpp surfaces: per glyph, the
 * GGO_GRAY4_BITMAP coverage (0..16), which with the blend in
 * src/platform/text.c reproduces Wine's text exactly (tools/probe/aaprobe.c
 * checks the model).  Same fonts, in the same order, as fontdump.c.
 *
 * File format (little endian): "C2FONTAA" u32 nfonts, then per font the same
 * 52-byte header as fonts.bin, then per char 0..255: i16 advance, i8 x,
 * i8 y (top left of the coverage bitmap relative to the cell's top left),
 * u8 w, u8 h, u16 n, n bytes of run-length coded coverage (row by row):
 * a byte b < 0x80 is followed by b+1 literal values; b >= 0x80 by one value
 * repeated b-0x80+2 times.
 * Run with tools/probe/run.sh fontdump_aa OUTFILE (fonts_win_aa.bin: with
 * the Liberation fonts installed, as fontdump.c says).
 */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

static void put32(FILE *f, int v) { fwrite(&v, 4, 1, f); }

static int rle(const BYTE *in, int n, BYTE *out)
{
    int i = 0, o = 0;
    while (i < n) {
        int run = 1;
        while (i + run < n && in[i + run] == in[i] && run < 129)
            run++;
        if (run >= 2) {
            out[o++] = (BYTE)(0x80 + run - 2);
            out[o++] = in[i];
            i += run;
        } else {
            int start = i, len = 0;
            while (i < n && len < 128 && !(i + 1 < n && in[i + 1] == in[i])) {
                i++;
                len++;
            }
            if (!len) {         /* a single value before a run */
                i++;
                len = 1;
            }
            out[o++] = (BYTE)(len - 1);
            memcpy(out + o, in + start, (size_t)len);
            o += len;
        }
    }
    return o;
}

int main(int argc, char **argv)
{
    static const char *faces[] = {"Times New Roman", "Arial", "System", "Courier"};
    static const BYTE family[] = {0x10, 0x20, 0x00, 0x30};
    MAT2 m = {{0, 1}, {0, 0}, {0, 0}, {0, 1}};
    HDC dc = CreateCompatibleDC(NULL);
    FILE *out = fopen(argc > 1 ? argv[1] : "fonts_aa.bin", "wb");
    int face, height, bold, italic, c;
    BYTE *buf = malloc(1 << 20), *pk = malloc(1 << 21), *cov = malloc(1 << 20);

    fwrite("C2FONTAA", 8, 1, out);
    put32(out, 4 * 43 * 4);
    for (face = 0; face < 4; face++)
        for (height = 6; height <= 48; height++)
            for (bold = 0; bold < 2; bold++)
                for (italic = 0; italic < 2; italic++) {
                    LOGFONTA lf;
                    TEXTMETRICA tm;
                    HFONT font, old;
                    unsigned char hdr[8] = {0};
                    memset(&lf, 0, sizeof lf);
                    lf.lfHeight = -height; lf.lfWeight = bold ? 700 : 400; lf.lfItalic = (BYTE)italic;
                    lf.lfCharSet = 1; lf.lfOutPrecision = 4; lf.lfClipPrecision = 1;
                    lf.lfPitchAndFamily = family[face];
                    strcpy(lf.lfFaceName, faces[face]);
                    font = CreateFontIndirectA(&lf);
                    old = SelectObject(dc, font);
                    GetTextMetricsA(dc, &tm);
                    hdr[0] = (BYTE)face; hdr[1] = (BYTE)height; hdr[2] = (BYTE)(lf.lfWeight & 0xff);
                    hdr[3] = (BYTE)(lf.lfWeight >> 8); hdr[4] = (BYTE)italic;
                    fwrite(hdr, 8, 1, out);
                    put32(out, tm.tmHeight); put32(out, tm.tmAscent); put32(out, tm.tmDescent);
                    put32(out, tm.tmInternalLeading); put32(out, tm.tmExternalLeading);
                    put32(out, tm.tmAveCharWidth); put32(out, tm.tmMaxCharWidth); put32(out, tm.tmWeight);
                    put32(out, tm.tmOverhang);
                    put32(out, tm.tmFirstChar | tm.tmLastChar << 8 | tm.tmDefaultChar << 16 | tm.tmBreakChar << 24);
                    put32(out, tm.tmPitchAndFamily | tm.tmCharSet << 8);
                    for (c = 0; c < 256; c++) {
                        GLYPHMETRICS gm;
                        char ch = (char)c;
                        SIZE sz;
                        DWORD n = GetGlyphOutlineA(dc, (unsigned char)c, GGO_GRAY4_BITMAP, &gm, 0, NULL, &m);
                        int w = 0, h = 0, x, y, stride, packed = 0;
                        short adv;
                        signed char xy[2] = {0, 0};
                        unsigned char wh[2];
                        unsigned short np;
                        GetTextExtentPoint32A(dc, &ch, 1, &sz);
                        adv = (short)sz.cx;
                        if (n != GDI_ERROR && n && n < (1 << 20)) {
                            GetGlyphOutlineA(dc, (unsigned char)c, GGO_GRAY4_BITMAP, &gm, n, buf, &m);
                            w = (int)gm.gmBlackBoxX;
                            h = (int)gm.gmBlackBoxY;
                            stride = (w + 3) & ~3;
                            for (y = 0; y < h; y++)
                                for (x = 0; x < w; x++)
                                    cov[y * w + x] = buf[y * stride + x];
                            packed = rle(cov, w * h, pk);
                            xy[0] = (signed char)gm.gmptGlyphOrigin.x;
                            xy[1] = (signed char)(tm.tmAscent - gm.gmptGlyphOrigin.y);
                        }
                        wh[0] = (unsigned char)w; wh[1] = (unsigned char)h;
                        np = (unsigned short)packed;
                        fwrite(&adv, 2, 1, out); fwrite(xy, 2, 1, out); fwrite(wh, 2, 1, out); fwrite(&np, 2, 1, out);
                        fwrite(pk, 1, (size_t)packed, out);
                    }
                    SelectObject(dc, old);
                    DeleteObject(font);
                }
    fclose(out);
    printf("done\n");
    return 0;
}
