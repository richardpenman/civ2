/*
 * Functions whose decompiled form took a register argument.  It was only a
 * decompiler artifact:
 *
 *   EAX  - the function returns a byte (or nothing) and the upper bits of
 *          EAX on some path are whatever the caller left there; the callers
 *          never look at them, so they are 0 here.
 *   AL   - an asm loop keeps the current audio sample in AL and may store
 *          it before loading the first one; it starts at 0 here.
 *   ECX  - the CRT strrchr (0x005f3c10) is typed __thiscall by Ghidra;
 *          it is really a plain __cdecl function and ECX is unused.
 */
#include "game.h"

/* CRT strrchr: __cdecl, caller cleans (protos.h wrongly says __thiscall). */
#define crt_strrchr _strrchr


typedef int (__stdcall *com_fn1)(int *);
typedef int (__stdcall *com_fn2)(int *, int);
#define COM_METHOD(obj, type, off) (*(type *)(*(obj) + (off)))

/* ------------------------------------------------------------------------ */

/* 0x0043d289: set (on != 0) or clear improvement bit `impr` (1..34) of a
 * city.  Really void; EAX ends up as city*11 (0 if impr is out of range). */
int __cdecl city_set_improvement(int city, int impr, int on)
{
    int index, mask;

    if (impr <= 0 || impr >= 0x23)
        return 0;
    bit_index_split(impr, &index, &mask);
    if (on != 0)
        cities[city].improvements[index] |= (byte)mask;
    else
        cities[city].improvements[index] &= (byte)~mask;
    return city * 0xb;
}

/* ------------------------------------------------------------------------ */

/* 0x005ea578: surface->GetFlipStatus(DDGFS_CANFLIP) == DD_OK (result in AL). */
uint __cdecl ddsurf_iface_can_flip(int *surface)
{
    uint hr;

    if (surface == NULL)
        return 0;
    hr = COM_METHOD(surface, com_fn2, 0x48)(surface, 1);
    if (hr == 0)
        return 1;
    return hr & 0xffffff00;
}

/* 0x005ea677: IDirectDraw::GetVerticalBlankStatus; AL = in vertical blank
 * (0 on failure or without DirectDraw). */
uint __cdecl ddraw_get_vblank_status(void)
{
    int *ddraw = (int *)g_ddraw;
    uint hr;
    int in_vblank;

    if (ddraw == NULL)
        return 0;
    hr = COM_METHOD(ddraw, com_fn2, 0x44)(ddraw, (iptr)&in_vblank);
    if (hr != 0)
        return hr & 0xffffff00;
    return (byte)in_vblank;
}

/* 0x005ef58e: IDirectDraw::RestoreDisplayMode; AL = succeeded. */
uint __cdecl ddraw_restore_display_mode(void)
{
    int *ddraw = (int *)g_ddraw;
    uint hr;

    if (ddraw == NULL)
        return 0;
    hr = COM_METHOD(ddraw, com_fn1, 0x4c)(ddraw);
    if (hr != 0)
        return hr & 0xffffff00;
    return 1;
}

/* ------------------------------------------------------------------------ */

/*
 * Sample-rate conversion of 8-bit unsigned audio into signed samples.  Each
 * of `count` steps (count 0 means 2^32) reads `src_step` source bytes and
 * emits `dst_step` output bytes, interleaved one read / one write at a time;
 * each output is the most recently read sample (initially 0).  Returns the
 * number of source bytes consumed.
 */

/* 0x005edc6c: store the samples. */
short __cdecl snd_resample_8bit(byte *dst, byte *src, uint count,
                                i32 src_step, i32 dst_step)
{
    uint n = (ushort)count;
    int nread = 0;
    byte sample = 0;
    byte reads, writes;

    do {
        reads = (byte)src_step;
        writes = (byte)dst_step;
        for (;;) {
            if (reads != 0) {
                sample = *src++ ^ 0x80;
                nread++;
                reads--;
            } else if (writes == 0) {
                break;
            }
            if (writes != 0) {
                *dst++ = sample;
                writes--;
            }
        }
    } while (--n != 0);
    return (short)nread;
}

/* 0x005edcac: mix the samples into dst with signed saturation. */
short __cdecl snd_mix_8bit_resample(char *dst_chars, byte *src, uint count,
                                    i32 src_step, i32 dst_step)
{
    signed char *dst = (signed char *)dst_chars;
    uint n = (ushort)count;
    int nread = 0;
    signed char sample = 0;
    byte reads, writes;
    int sum;

    do {
        reads = (byte)src_step;
        writes = (byte)dst_step;
        for (;;) {
            if (reads != 0) {
                sample = (signed char)(*src++ ^ 0x80);
                reads--;
                nread++;
            } else if (writes == 0) {
                break;
            }
            if (writes != 0) {
                sum = *dst + sample;
                if (sum > 0x7f)
                    sum = 0x7f;
                else if (sum < -0x80)
                    sum = -0x80;
                *dst++ = (signed char)sum;
                writes--;
            }
        }
    } while (--n != 0);
    return (short)nread;
}

/* ------------------------------------------------------------------------ */

/* 0x00472950: append "." + ext if the file name has no extension, then
 * uppercase it. */
void __cdecl path_default_extension(char *name, char *ext)
{
    if (crt_strrchr(name, '.') == NULL) {
        crt_strcat(name, ((i8 *)&path_default_ext_dot_str));
        crt_strcat(name, ext);
    }
    __strupr(name);
}

/* 0x004729ab: dst = src with its extension replaced by ext, uppercased. */
void __cdecl path_set_extension(char *dst, char *src, char *ext)
{
    char *dot;

    if (src != dst)
        crt_strcpy(dst, src);
    dot = crt_strrchr(dst, '.');
    if (dot != NULL)
        *dot = '\0';
    crt_strcat(dst, ((i8 *)&path_set_ext_dot_str));
    crt_strcat(dst, ext);
    __strupr(dst);
}

/* 0x0056b810: cut the string at its last newline; returns where (or NULL). */
char * __cdecl str_strip_newline(char *s)
{
    char *nl;

    nl = crt_strrchr(s, '\n');
    if (nl != NULL)
        *nl = '\0';
    return nl;
}
