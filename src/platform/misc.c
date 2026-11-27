/*
 * Services the native build does not provide (DirectDraw, AVI video),
 * message boxes, file dialogs, common controls.
 */
#include "plat.h"
#include "../host/host.h"

/* ---- DirectDraw: not available; the game uses GDI ---- */

HRESULT WINAPI DirectDrawCreate(GUID *guid, LPDIRECTDRAW *dd, IUnknown *outer)
{
    API_TRACE();
    (void)guid; (void)outer;
    if (dd)
        *dd = NULL;
    return (HRESULT)0x88760255u;     /* DDERR_NODIRECTDRAWHW */
}

/* AVI files and the IV41 codec: video.c */

HWND MCIWndCreateA(HWND parent, HINSTANCE inst, DWORD style, LPCSTR file)
{
    API_TRACE();
    (void)parent; (void)inst; (void)style; (void)file;
    return 0;
}

void Ordinal_17(void) {}

/* ---- message boxes ---- */

int WINAPI MessageBoxA(HWND owner, LPCSTR text, LPCSTR caption, UINT type)
{
    API_TRACE();
    const char *labels[3];
    int ids[3], n = 0, enter = 0, escape, hit;
    (void)owner;
    plat_log("MessageBox: %s: %s\n", caption ? caption : "", text ? text : "");
    switch (type & MB_TYPEMASK) {
    case MB_OKCANCEL:
        labels[n] = extras_text("box.ok"); ids[n++] = IDOK;
        labels[n] = extras_text("box.cancel"); ids[n++] = IDCANCEL;
        break;
    case MB_YESNO:
        labels[n] = extras_text("box.yes"); ids[n++] = IDYES;
        labels[n] = extras_text("box.no"); ids[n++] = IDNO;
        break;
    case MB_YESNOCANCEL:
        labels[n] = extras_text("box.yes"); ids[n++] = IDYES;
        labels[n] = extras_text("box.no"); ids[n++] = IDNO;
        labels[n] = extras_text("box.cancel"); ids[n++] = IDCANCEL;
        break;
    default:
        labels[n] = extras_text("box.ok"); ids[n++] = IDOK;
    }
    escape = n - 1;
    if (plat_replay)
        return ids[0];
    hit = host_message_box((type & MB_ICONMASK) == MB_ICONERROR ? 2 : (type & MB_ICONMASK) == MB_ICONWARNING ? 1 : 0,
                           caption ? caption : "OpenCiv2", text ? text : "", n, labels, ids, enter, escape);
    return hit < 0 ? ids[n - 1] : hit;
}

WINBOOL WINAPI MessageBeep(UINT type) { API_TRACE(); (void)type; return TRUE; }

/* ---- file dialogs ---- */

DWORD WINAPI CommDlgExtendedError(void) { API_TRACE(); return 0; }

/* Without a native dialog, the scripted answer (replay) or a name typed
 * on the terminal. */
static WINBOOL file_dialog(OPENFILENAMEA *ofn, const char *kind)
{
    static int real = -1;
    if (real < 0)
        real = getenv("CIV2_REAL_FILEDLG") != NULL;     /* test the dialog itself in a replay */
    if (plat_replay && !real)
        return replay_file_dialog(ofn, kind);
    return plat_file_dialog(ofn, !strcmp(kind, "Save"));
}

WINBOOL WINAPI GetOpenFileNameA(LPOPENFILENAMEA ofn) { API_TRACE(); return file_dialog(ofn, "Open"); }
WINBOOL WINAPI GetSaveFileNameA(LPOPENFILENAMEA ofn) { API_TRACE(); return file_dialog(ofn, "Save"); }

/* XDaemon (network play): xdaemon.c */
