// SOURCE src/platform/gdi.c bmp_of_bits
#include <cassert>
#include <cstdint>
#include <cstdio>
using Handle = uintptr_t;
enum { OBJ_BITMAP=5 };
struct Bitmap { unsigned char *bits; };
static Bitmap *objects[8192];
static Bitmap *obj_get(Handle h,int kind) {
    return kind==OBJ_BITMAP && h>>16==OBJ_BITMAP ? objects[h & 65535] : nullptr;
}
#include "production.inc"
int main() {
    unsigned char data[4];
    Bitmap first={data},second={data+1};
    objects[17]=&first;
    assert(bmp_of_bits(data)==&first);
    assert(bmp_of_bits(data)==&first); // cached
    objects[17]=nullptr;
    assert(!bmp_of_bits(data)); // deleted cached handle
    objects[17]=&second;
    assert(!bmp_of_bits(data)); // handle reused with different storage
    first.bits=data+2;objects[19]=&first;
    assert(bmp_of_bits(data+2)==&first);
    first.bits=data+3;
    assert(!bmp_of_bits(data+2)); // cached bitmap's DIB storage replaced
    assert(bmp_of_bits(data+3)==&first);
    second.bits=data+2;
    assert(bmp_of_bits(data+2)==&second); // freed address reused by other handle
    assert(!bmp_of_bits(nullptr));
    puts("Bitmap lookup: deletion, handle reuse and DIB replacement passed");
}
