/*
 * Capture the antialiased form of the game's fonts as the reference
 * environment (Wine) uses it on 32-bpp surfaces: per glyph, the
 * GGO_GRAY4_BITMAP coverage (0..16), which with the blend in
 * src/platform/text.c reproduces Wine's text exactly (tools/probe/aaprobe.c
 * checks the model).  Same fonts, in the same order, as fontdump.c.
 *
 * File format (little endian): "C2FONTAA" u32 nfonts, then per font the same
 * 52-byte header as fonts_win.bin (but byte 5: 1 where drawn without
 * antialiasing, see drawn_mono), then per char 0..255: i16 advance (where
 * drawn without antialiasing: the drawing advance, right_ink), i8 x,
 * i8 y (top left of the coverage bitmap relative to the cell's top left),
 * u8 w, u8 h, u16 n, n bytes of run-length coded coverage (row by row):
 * a byte b < 0x80 is followed by b+1 literal values; b >= 0x80 by one value
 * repeated b-0x80+2 times.
 * Run with tools/probe/run.sh fontdump_aa OUTFILE (data/native/fonts_win_aa.bin).
 */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define W 192
#define H 128
#define OX 48
#define OY 32

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

/* Wine draws without antialiasing the sizes the font's "gasp" table says
   not to (Liberation's middle sizes), and those glyphs differ by a pixel
   here and there from the ones it draws on 8-bpp surfaces, though they
   advance as those (fonts_win.bin's drawing advances).  Such a font is
   marked in its header (byte 5) and its glyphs are as drawn on a 32-bpp
   surface, black on white: coverage 0 or 16 (top left relative to the
   cell's).  The character drawn: if it has no grey, 1. */
static HDC mdc32;
static DWORD *px32;

static int drawn_mono(HFONT font, int c, BYTE *cov, int *w, int *h, signed char *xy)
{
#define mdc mdc32
#define px px32
    int x, y, x0 = W, y0 = H, x1 = -1, y1 = -1;
    char ch = (char)c;
    if (!mdc) {
        BITMAPINFO bi;
        memset(&bi, 0, sizeof bi);
        bi.bmiHeader.biSize = sizeof bi.bmiHeader;
        bi.bmiHeader.biWidth = W; bi.bmiHeader.biHeight = -H;
        bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32;
        mdc = CreateCompatibleDC(NULL);
        SelectObject(mdc, CreateDIBSection(mdc, &bi, DIB_RGB_COLORS, (void **)&px, NULL, 0));
        SetBkMode(mdc, TRANSPARENT);
        SetTextColor(mdc, RGB(0, 0, 0));
    }
    SelectObject(mdc, font);
    for (y = 0; y < W * H; y++)
        px[y] = 0xffffff;
    TextOutA(mdc, OX, OY, &ch, 1);
    GdiFlush();
    for (y = 0; y < H; y++)
        for (x = 0; x < W; x++) {
            DWORD v = px[y * W + x] & 0xffffff;
            if (v == 0xffffff)
                continue;
            if (v != 0)
                return 0;               /* grey: antialiased */
            if (x < x0) x0 = x;
            if (x > x1) x1 = x;
            if (y < y0) y0 = y;
            if (y > y1) y1 = y;
        }
    if (x1 < 0 || !cov)
        return x1 >= 0;                 /* nothing drawn: as GetGlyphOutline says */
    *w = x1 - x0 + 1;
    *h = y1 - y0 + 1;
    for (y = 0; y < *h; y++)
        for (x = 0; x < *w; x++)
            cov[y * *w + x] = (px[(y0 + y) * W + x0 + x] & 0xffffff) ? 0 : 16;
    xy[0] = (signed char)(x0 - OX);
    xy[1] = (signed char)(y0 - OY);
    return 1;
#undef mdc
#undef px
}

/* The drawing advance on a 32-bpp surface of a font drawn without
   antialiasing: where a following 'H' lands (its rightmost ink), as
   fontdump.c measures it on 8-bpp ones, where it can differ. */
static int right_ink(HFONT font, const char *s, int n)
{
    int x, y, r = -1;
    drawn_mono(font, 'H', NULL, NULL, NULL, NULL);     /* sets up the surface */
    SelectObject(mdc32, font);
    for (y = 0; y < W * H; y++)
        px32[y] = 0xffffff;
    TextOutA(mdc32, OX, OY, s, n);
    GdiFlush();
    for (y = 0; y < H; y++)
        for (x = 0; x < W; x++)
            if ((px32[y * W + x] & 0xffffff) != 0xffffff && x > r)
                r = x;
    return r;
}

int main(int argc, char **argv)
{
    static const char *faces[] = {"Times New Roman", "Arial", "System", "Courier"};
    static const BYTE family[] = {0x10, 0x20, 0x00, 0x30};
    MAT2 m = {{0, 1}, {0, 0}, {0, 0}, {0, 1}};
    HDC dc = CreateCompatibleDC(NULL);
    FILE *out = fopen(argc > 1 ? argv[1] : "fonts_win_aa.bin", "wb");
    int face, height, bold, italic, c, mono;
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
                    {   /* drawn without antialiasing ("gasp"): O and a have no grey */
                        int ww, hh;
                        signed char t[2];
                        mono = drawn_mono(font, 'O', cov, &ww, &hh, t) && drawn_mono(font, 'a', cov, &ww, &hh, t);
                    }
                    hdr[5] = (BYTE)mono;
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
                        if (mono) {
                            char pair[2];
                            pair[0] = ch; pair[1] = 'H';
                            adv = (short)(right_ink(font, pair, 2) - right_ink(font, "H", 1));  /* as drawn */
                            if (drawn_mono(font, (unsigned char)c, cov, &w, &h, xy))
                                packed = rle(cov, w * h, pk);   /* as drawn */
                        } else if (n != GDI_ERROR && n && n < (1 << 20)) {
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
