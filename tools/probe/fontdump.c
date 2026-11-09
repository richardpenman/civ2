/*
 * Capture the game's fonts exactly as the reference environment (Wine, with
 * the DejaVu fonts it substitutes for Times New Roman, Arial, System and
 * Courier) draws them on 8-bpp surfaces, where glyphs are 1-bit: every
 * character of every face, height, weight and slant the game can ask for.
 * The native platform layer draws text from this file (data/fonts.bin), so
 * its text is pixel-identical to the original's.
 *
 * Text is laid out with the advances GetTextExtentPoint reports, but Wine
 * draws glyphs with its own advances, which for small sizes can differ by a
 * pixel: both are recorded.
 *
 * File format (little endian):
 *   "C2FONTS2" u32 nfonts
 *   per font: u8 face, u8 height(-lfHeight), u16 weight, u8 italic, u8 pad[3],
 *             i32 tm[11]  (height ascent descent intlead extlead avechar maxchar
 *                          weight overhang firstchar|lastchar<<8|default<<16|break<<24
 *                          pitchfamily|charset<<8)
 *             per char 0..255: i16 advance (extent), i16 drawing advance, i8 x,
 *             i8 y, u8 w, u8 h, then
 *             (w+7)/8*h bytes of bits (msb first), x,y = top left of the
 *             bitmap relative to the text origin (top left of the cell)
 * Run with tools/probe/run.sh fontdump OUTFILE.
 *
 * data/native/fonts_win.bin (played with outside replays and classic mode:
 * src/platform/text.c, open_fonts) is the same capture with the Liberation 2
 * fonts installed (Debian's fonts-liberation2), which Wine then draws in
 * place of Times New Roman, Arial and Courier: their widths are those of
 * the Windows fonts, so text is laid out as on Windows.  In the civ2-dev
 * container: apt-get install fonts-liberation2 (as root), then run.sh as
 * the user dev (Wine's prefix is his).  Its licence: THIRD-PARTY.txt.
 */
#include <windows.h>
#include <stdio.h>

#define W 192
#define H 128
#define OX 48
#define OY 32

static void put32(FILE *f, int v) { fwrite(&v, 4, 1, f); }

int main(int argc, char **argv)
{
    static const char *faces[] = {"Times New Roman", "Arial", "System", "Courier"};
    static const BYTE family[] = {0x10, 0x20, 0x00, 0x30};
    struct { BITMAPINFOHEADER h; RGBQUAD c[256]; } bi;
    unsigned char *bits;
    HDC dc = CreateCompatibleDC(NULL);
    FILE *out = fopen(argc > 1 ? argv[1] : "fonts.bin", "wb");
    int face, height, bold, italic, n = 0, c;

    memset(&bi, 0, sizeof bi);
    bi.h.biSize = sizeof bi.h; bi.h.biWidth = W; bi.h.biHeight = -H; bi.h.biPlanes = 1; bi.h.biBitCount = 8;
    bi.c[1].rgbRed = bi.c[1].rgbGreen = bi.c[1].rgbBlue = 255;
    SelectObject(dc, CreateDIBSection(dc, (BITMAPINFO *)&bi, DIB_RGB_COLORS, (void **)&bits, NULL, 0));
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, RGB(0, 0, 0));
    SetTextAlign(dc, TA_TOP | TA_LEFT);

    fwrite("C2FONTS2", 8, 1, out);
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
                        char ch = (char)c;
                        SIZE sz;
                        int x, y, x0 = W, y0 = H, x1 = -1, y1 = -1;
                        short adv;
                        signed char rec[2];
                        unsigned char wh[2];
                        short dadv;
                        {
                            /* Drawing advance: where a following 'H' lands (its rightmost ink). */
                            char pair[2];
                            int right_h = -1, right_pair = -1;
                            pair[0] = ch; pair[1] = 'H';
                            memset(bits, 1, W * H);
                            TextOutA(dc, OX, OY, "H", 1);
                            for (y = 0; y < H; y++)
                                for (x = 0; x < W; x++)
                                    if (bits[y * W + x] == 0 && x > right_h)
                                        right_h = x;
                            memset(bits, 1, W * H);
                            TextOutA(dc, OX, OY, pair, 2);
                            for (y = 0; y < H; y++)
                                for (x = 0; x < W; x++)
                                    if (bits[y * W + x] == 0 && x > right_pair)
                                        right_pair = x;
                            dadv = (short)(right_pair - right_h);
                        }
                        memset(bits, 1, W * H);
                        TextOutA(dc, OX, OY, &ch, 1);
                        GetTextExtentPoint32A(dc, &ch, 1, &sz);
                        for (y = 0; y < H; y++)
                            for (x = 0; x < W; x++)
                                if (bits[y * W + x] == 0) {
                                    if (x < x0) x0 = x;
                                    if (x > x1) x1 = x;
                                    if (y < y0) y0 = y;
                                    if (y > y1) y1 = y;
                                }
                        if (x1 < 0) { x0 = OX; y0 = OY; x1 = OX - 1; y1 = OY - 1; }
                        adv = (short)sz.cx;
                        rec[0] = (signed char)(x0 - OX); rec[1] = (signed char)(y0 - OY);
                        wh[0] = (unsigned char)(x1 - x0 + 1); wh[1] = (unsigned char)(y1 - y0 + 1);
                        fwrite(&adv, 2, 1, out); fwrite(&dadv, 2, 1, out);
                        fwrite(rec, 2, 1, out); fwrite(wh, 2, 1, out);
                        for (y = y0; y <= y1; y++) {
                            unsigned char row[32] = {0};
                            for (x = x0; x <= x1; x++)
                                if (bits[y * W + x] == 0)
                                    row[(x - x0) >> 3] |= (unsigned char)(0x80 >> ((x - x0) & 7));
                            fwrite(row, (x1 - x0 + 8) / 8, 1, out);
                        }
                    }
                    SelectObject(dc, old);
                    DeleteObject(font);
                    n++;
                }
    fclose(out);
    printf("fonts: %d\n", n);
    return 0;
}
