// SOURCE src/auto/graphics.cpp gfx_load_bmp_640x480
// SOURCE src/auto/graphics.cpp gfx_save_bmp_640x480
// Actual loader/writer bodies, with stdio files and a checked in-memory surface.
#include <cassert>
#include <cstdio>
#include <cstring>
#include <vector>
#include "native.h"
#include "win32_types.h"
#include "civ2types.h"
typedef unsigned int uint;
typedef unsigned char byte;
static_assert(sizeof(void *) == 4, "run with a 32-bit compiler");
static FILE *input;
static std::vector<unsigned char> output;
static const char *last_error;
static char gfx_load_bmp_no_error_str[] = "none", gfx_save_bmp_no_error_str[] = "none";
static char gfx_bmp_read_mode[] = "rb", gfx_bmp_write_mode[] = "wb";
#define ERROR_TEXT(name) static char name[] = #name
ERROR_TEXT(str_error_reading_file_header);
ERROR_TEXT(str_error_reading_info_header);
ERROR_TEXT(str_error_reading_file_palette);
ERROR_TEXT(str_error_reading_bitmap_image);
ERROR_TEXT(str_incompatable_compression_mode_bi_rle4);
ERROR_TEXT(str_image_not_640_x_480_x_256_color);
ERROR_TEXT(str_invalid_bitmap_file);
ERROR_TEXT(str_error_writing_file_header);
ERROR_TEXT(str_error_writing_info_header);
ERROR_TEXT(str_error_writing_file_palette);
ERROR_TEXT(str_error_writing_bitmap_image);
static FILE *__fsopen(char *, char *, int) {
    rewind(input);
    return input;
}
static size_t _fread(void *p, size_t n, size_t count, FILE *f) { return fread(p, n, count, f); }
static size_t _fwrite(const void *p, size_t n, size_t count, FILE *) {
    const unsigned char *b = (const unsigned char *)p;
    output.insert(output.end(), b, b + n * count);
    return count;
}
static int _fputc(int c, FILE *) {
    output.push_back((unsigned char)c);
    return c & 255;
}
static int _fclose(FILE *) { return 0; } // fixture owns the file
#define CRT_GETC(f) fgetc(f)
static int MessageBoxA(HWND, const char *text, LPCSTR, UINT) {
    last_error = text;
    return 0;
}
struct DibSurface {
    unsigned char pixels[480][640];
    void create(int w, int h) {
        assert(w == 640 && h == 480);
        memset(pixels, 0, sizeof pixels);
    }
    iptr pixel_ptr(int x, int y) {
        assert(y >= 0 && y < 480); // the row table's entries (x may run on)
        return (iptr)&pixels[0][0] + y * 640 + x;
    }
};
struct GamePalette {
    GamePalette *get_logpalette() { return this; }
    void commit_range(int, int) {}
};
static void palette_set_entry_rgb(GameLogPalette *, int, int, int, int) {}
static void palette_get_entry_rgb(GameLogPalette *, int color, iptr r, iptr g, iptr b) {
    *(char *)r = (char)color;
    *(char *)g = (char)(color + 1);
    *(char *)b = (char)(color + 2);
}
#include "production.inc"
static void put32(std::vector<unsigned char> &v, size_t off, int value) {
    memcpy(v.data() + off, &value, 4);
}
static void bmp(const std::vector<unsigned char> &rle, int info_size = 40) {
    std::vector<unsigned char> bytes(14 + 40 + 1024);
    bytes[0] = 'B';
    bytes[1] = 'M';
    put32(bytes, 14, info_size);
    put32(bytes, 18, 640);
    put32(bytes, 22, 480);
    bytes[26] = 1;
    bytes[28] = 8;
    put32(bytes, 30, 1);
    bytes.insert(bytes.end(), rle.begin(), rle.end());
    input = tmpfile();
    assert(input);
    assert(fwrite(bytes.data(), 1, bytes.size(), input) == bytes.size());
    rewind(input);
    last_error = nullptr;
}
int main() {
    DibSurface surface{};
    bmp({0, 2, 1, 3, 2, 7, 0, 1}); // original decoder's vertical-first delta: (3,478)
    assert(gfx_load_bmp_640x480((iptr)&surface, "input", 0, 256, 0) == 1);
    assert(!last_error && surface.pixels[478][3] == 7 && surface.pixels[478][4] == 7);
    fclose(input);
    bmp({2, 9, 0, 1});
    assert(gfx_load_bmp_640x480((iptr)&surface, "input", 0, 256, 0) == 1);
    assert(surface.pixels[479][0] == 9 && surface.pixels[479][1] == 9);
    fclose(input);
    GamePalette palette;
    for (int custom = 0; custom < 2; custom++) {
        input = tmpfile();
        assert(input);
        output.clear();
        assert(gfx_save_bmp_640x480((iptr)&surface, "output", 0, 256,
                                    custom ? (iptr)&palette : 0) == 1);
        assert(output.size() == 308278);
        for (int c = 0; c < 256; c++) {
            assert(output[54 + c * 4 + 3] == 0);
            if (custom) {
                assert(output[54 + c * 4] == (unsigned char)(c + 2));
                assert(output[54 + c * 4 + 2] == c);
            }
        }
        assert(output[1078] == 9 && output[1079] == 9); // first bottom-up row unchanged
        fclose(input);
    }
    // BF-097: an info header too short for the compression read below.
    bmp({2, 9, 0, 1}, 16);
    assert(gfx_load_bmp_640x480((iptr)&surface, "input", 0, 256, 0) == 0);
    assert(last_error && strcmp(last_error, "str_error_reading_info_header") == 0);
    fclose(input);
    // BF-098: writes outside the image refused; an end of line past the last
    // row (as encoders write) and the end of bitmap still accepted.
    std::vector<unsigned char> lines;
    for (int r = 0; r < 480; r++)
        lines.insert(lines.end(), {1, 4, 0, 0});
    lines.insert(lines.end(), {0, 1});
    bmp(lines);
    assert(gfx_load_bmp_640x480((iptr)&surface, "input", 0, 256, 0) == 1 && !last_error);
    assert(surface.pixels[0][0] == 4 && surface.pixels[479][0] == 4);
    fclose(input);
    std::vector<std::vector<unsigned char>> bad = {
        {0, 2, 255, 0, 0, 2, 255, 0, 1, 7, 0, 1},       // a delta below the image, then a run
        {0, 2, 0, 255, 0, 2, 0, 255, 255, 7, 0, 1},     // a run past the image's end
        {0, 2, 0, 255, 0, 2, 0, 255, 0, 2, 0, 127, 0, 5, 1, 2, 3, 4, 5, 0, 0, 1}, // a literal
    };
    std::vector<unsigned char> past; // a run after an end of line past the last row
    for (int r = 0; r < 480; r++)
        past.insert(past.end(), {0, 0});
    past.insert(past.end(), {1, 1, 0, 1});
    bad.push_back(past);
    for (auto &rle : bad) {
        bmp(rle);
        assert(gfx_load_bmp_640x480((iptr)&surface, "input", 0, 256, 0) == 0);
        assert(last_error && strcmp(last_error, "str_error_reading_bitmap_image") == 0);
        fclose(input);
    }
    puts("BMP: initial delta, encoded run, short info header, writes outside the image, "
         "default/custom palettes and pixel output passed");
}
