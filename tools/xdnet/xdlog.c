/* Logging proxy for the original XDaemon.dll (renamed XDaemon_real.dll):
   logs every interesting XD_* call with its arguments and payload bytes,
   and wraps the game's receive callbacks to log what arrives.
   Log file: %XDLOG% (Windows path), default xdlog.txt in the game dir. */
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>

void *xd_real[64];
#include "stubs.h"

static HANDLE logf_h = INVALID_HANDLE_VALUE;
static void timers(void);
static CRITICAL_SECTION lk;
static DWORD t0;

static void lg(const char *fmt, ...)
{
    char buf[8192]; int n; DWORD w; va_list ap;
    timers();
    n = snprintf(buf, 64, "%7lu t%04lx ", GetTickCount() - t0, GetCurrentThreadId() & 0xffff);
    va_start(ap, fmt); n += vsnprintf(buf + n, sizeof buf - n - 2, fmt, ap); va_end(ap);
    if (n > (int)sizeof buf - 2) n = sizeof buf - 2;
    buf[n++] = '\n';
    EnterCriticalSection(&lk);
    WriteFile(logf_h, buf, n, &w, NULL);
    LeaveCriticalSection(&lk);
}

static void hex(const char *tag, const void *p, unsigned long len)
{
    const unsigned char *b = p; unsigned long i, max = len > 4096 ? 4096 : len;
    char line[200];
    if (!p) return;
    for (i = 0; i < max; i += 32) {
        int n = 0; unsigned long j;
        for (j = i; j < i + 32 && j < max; j++) n += sprintf(line + n, "%02x", b[j]);
        lg("  %s %04lx: %s", tag, i, line);
    }
    if (max < len) lg("  %s ... (%lu bytes total)", tag, len);
}

#define REAL(name) ((void *)xd_real[IDX_##name])

/* XDTIMERS=1: the game's timers (timerdll.dll, src/platform/resources.c has
   the same five functions).  timerdll's TimerCallBack, on the multimedia
   timer's thread, posts 0x52c (wparam: the game's timer slot) to the timer
   window, then sets the slot's "notified" flag; the window's 0x52c handler
   fires the slot and clears the flag (ResetTimerNotified); while the flag
   is set the slot posts nothing.  Logged: each 0x52c the timer window gets
   (the timer window subclassed: the flag when the handler starts, 0 when
   it runs before TimerCallBack has set it: TimerCallBack then sets it,
   and if that comes after the handler has cleared it the flag stays set
   with no message coming, and the slot never posts again), and the slots'
   timer ids and flags each time they change (looked at on every XD call,
   the game's message polls included: XD_InFlushSendBuffer). */
static int timers_on = -1, in_timers;
static volatile DWORD *td_ids, *td_flags;
static HWND td_hwnd;
static HMODULE td_module;
static WNDPROC td_proc;
static DWORD td_last_ids[17], td_last_flags[17];
static LRESULT CALLBACK td_wndproc(HWND h, UINT m, WPARAM w, LPARAM l)
{
    DWORD before, after, t;
    LRESULT r;
    if (m != 0x52c || w >= 16 || !td_flags)
        return CallWindowProcA(td_proc, h, m, w, l);
    before = td_flags[w];
    t = GetTickCount();
    lg("timer 0x52c slot %u (id %lu), flag %lu%s", (unsigned)w, td_ids[w], before,
       before ? "" : "   (not set yet: if TimerCallBack sets it after this handler has cleared it, the slot never posts again)");
    r = CallWindowProcA(td_proc, h, m, w, l);
    after = td_flags[w];
    t = GetTickCount() - t;
    if (t > 100 || after)
        lg("timer 0x52c slot %u handled in %lu ms, flag now %lu (id %lu)", (unsigned)w, t, after, td_ids[w]);
    return r;
}
static void timers(void)
{
    HMODULE m;
    char buf[600];
    int i, n = 0, changed = 0;
    if (timers_on < 0) {
        char v[8];
        timers_on = GetEnvironmentVariableA("XDTIMERS", v, sizeof v) && v[0] == '1';
    }
    if (!timers_on || in_timers)
        return;
    in_timers = 1;
    m = GetModuleHandleA("timerdll.dll");
    if (m != td_module) {       /* (loaded with the game's timer window, freed with it) */
        td_module = m;
        td_ids = td_flags = NULL;
        td_hwnd = NULL;
    }
    if (td_hwnd && !IsWindow(td_hwnd))
        td_hwnd = NULL;
    if (!td_ids && m != NULL) {
        BYTE *a = (BYTE *)m + 0x105c, *b = (BYTE *)m + 0x10df;   /* SetTimerID's and ResetTimerNotified's stores */
        if (a[0] == 0x89 && a[1] == 0x04 && a[2] == 0x8d && b[0] == 0xc7 && b[1] == 0x04 && b[2] == 0x85) {
            td_ids = *(DWORD **)(a + 3);
            td_flags = *(DWORD **)(b + 3);
        }
        lg("timerdll.dll at %p: timer ids %p, flags %p", m, td_ids, td_flags);
    }
    if (td_ids && !td_hwnd && (td_hwnd = FindWindowA("MSMrTimerClass", "MrTimer")) != NULL) {
        td_proc = (WNDPROC)SetWindowLongA(td_hwnd, GWL_WNDPROC, (LONG)td_wndproc);
        lg("timer window %p subclassed (its procedure %p)", td_hwnd, td_proc);
    }
    if (td_ids) {
        for (i = 1; i < 17; i++)
            if (td_ids[i] != td_last_ids[i] || td_flags[i] != td_last_flags[i])
                changed = 1;
        if (changed) {
            for (i = 1; i < 17; i++) {
                td_last_ids[i] = td_ids[i];
                td_last_flags[i] = td_flags[i];
                if (td_ids[i] || td_flags[i])
                    n += snprintf(buf + n, sizeof buf - n, " %d:%lu%s", i, td_ids[i], td_flags[i] ? "*" : "");
            }
            buf[n] = 0;
            lg("timers (slot:id, * flag set):%s", n ? buf : " none");
        }
    }
    in_timers = 0;
}

/* Wine/Linux report SO_RCVBUF as 128K+; XDaemon stores the buffer size in a
   16-bit field, so it would recv() with length 0 forever.  Report Windows 9x's
   8192 instead (does not change anything on the wire).  CLAMP_RCVBUF=0 disables. */
typedef int (WINAPI *getsockopt_t)(unsigned, int, int, char *, int *);
static getsockopt_t real_getsockopt;
static int clamp_on = 1;
static int WINAPI my_getsockopt(unsigned s, int level, int name, char *val, int *len)
{
    int r = real_getsockopt(s, level, name, val, len);
    if (r == 0 && level == 0xffff && (name == 0x1002 || name == 0x1001) && *len >= 4) {
        int v = *(int *)val;
        if (clamp_on && v > 0xffff) *(int *)val = 8192;
        lg("getsockopt(sock=%u, %s) = %d -> %d", s, name == 0x1002 ? "SO_RCVBUF" : "SO_SNDBUF", v, *(int *)val);
    }
    return r;
}

static void patch_iat(HMODULE m)
{
    BYTE *base = (BYTE *)m;
    IMAGE_NT_HEADERS *nt = (IMAGE_NT_HEADERS *)(base + ((IMAGE_DOS_HEADER *)base)->e_lfanew);
    IMAGE_IMPORT_DESCRIPTOR *d = (IMAGE_IMPORT_DESCRIPTOR *)(base + nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
    for (; d->Name; d++) {
        IMAGE_THUNK_DATA *o, *f;
        if (lstrcmpiA((char *)base + d->Name, "WSOCK32.dll")) continue;
        o = (IMAGE_THUNK_DATA *)(base + d->OriginalFirstThunk); f = (IMAGE_THUNK_DATA *)(base + d->FirstThunk);
        for (; o->u1.Ordinal; o++, f++)
            if (IMAGE_SNAP_BY_ORDINAL(o->u1.Ordinal) && IMAGE_ORDINAL(o->u1.Ordinal) == 7) {
                DWORD old;
                real_getsockopt = (getsockopt_t)f->u1.Function;
                VirtualProtect(&f->u1.Function, 4, PAGE_READWRITE, &old);
                f->u1.Function = (DWORD)my_getsockopt;
                VirtualProtect(&f->u1.Function, 4, old, &old);
                lg("patched XDaemon's getsockopt import");
            }
    }
}

BOOL WINAPI DllMain(HINSTANCE h, DWORD reason, LPVOID r)
{
    if (reason == DLL_PROCESS_ATTACH) {
        char path[MAX_PATH]; HMODULE m; int i;
        InitializeCriticalSection(&lk);
        t0 = GetTickCount();
        if (!GetEnvironmentVariableA("XDLOG", path, sizeof path)) strcpy(path, "xdlog.txt");
        logf_h = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
        m = LoadLibraryA("XDaemon_orig.dll");
        if (!m) { lg("cannot load XDaemon_orig.dll"); return FALSE; }
        for (i = 0; i < NXD; i++) xd_real[i] = (void *)GetProcAddress(m, xd_names[i]);
        lg("proxy loaded, tick0=%lu", t0);
        if (GetEnvironmentVariableA("CLAMP_RCVBUF", path, sizeof path) && path[0] == '0') clamp_on = 0;
        patch_iat(m);
    }
    return TRUE;
}

/* ---- callbacks ---- */
typedef void (*BcastCB)(void *, unsigned short, long);
typedef void (*SecureCB)(unsigned short, void *, unsigned long, short);
typedef void (*NewClientCB)(unsigned short, unsigned short);
typedef void (*ConnCB)(short);
typedef void (*LostCB)(unsigned short);
typedef void (*OversizeCB)(unsigned long);
static BcastCB g_bcast, g_init_cb; static SecureCB g_secure; static NewClientCB g_newc;
static ConnCB g_conn; static LostCB g_lost; static OversizeCB g_over;

static void cb_bcast(void *m, unsigned short sz, long x)
{ lg("CB BroadcastReceive size=%u x=%ld", sz, x); hex("rx", m, sz); if (g_bcast) g_bcast(m, sz, x); }
static void cb_initcb(void *m, unsigned short sz, long x)
{ lg("CB InitCallback size=%u x=%ld", sz, x); hex("rx", m, sz); if (g_init_cb) g_init_cb(m, sz, x); }
static void cb_secure(unsigned short id, void *m, unsigned long sz, short x)
{ lg("CB SecureReceive from=%u size=%lu x=%d", id, sz, x); hex("rx", m, sz); if (g_secure) g_secure(id, m, sz, x); }
static void cb_newc(unsigned short a, unsigned short b)
{ lg("CB NewClientConnection a=%u b=%u", a, b); if (g_newc) g_newc(a, b); }
static void cb_conn(short id)
{ lg("CB ClientConnectionToServer id=%d", id); if (g_conn) g_conn(id); }
static void cb_lost(unsigned short id)
{ lg("CB ConnectionLost id=%u", id); if (g_lost) g_lost(id); }
static void cb_over(unsigned long sz)
{ lg("CB Oversized size=%lu", sz); if (g_over) g_over(sz); }

/* ---- wrapped calls (all __cdecl) ---- */
int w_XD_ActivateServer(void)
{ int r = ((int (*)(void))REAL(XD_ActivateServer))(); lg("XD_ActivateServer() = %d", r); return r; }
int w_XD_CloseConnection(void)
{ int r = ((int (*)(void))REAL(XD_CloseConnection))(); lg("XD_CloseConnection() = %d", r); return r; }
int w_XD_InFlushSendBuffer(void)
{ timers(); return ((int (*)(void))REAL(XD_InFlushSendBuffer))(); }
int w_XD_FlushSendBuffer(unsigned long t)
{ int r; lg("XD_FlushSendBuffer(%lu) ...", t); r = ((int (*)(unsigned long))REAL(XD_FlushSendBuffer))(t); lg("XD_FlushSendBuffer = %d", r); return r; }
char *w_XD_GetCurrentProtoAddr(unsigned a, int p)
{ char *r = ((char *(*)(unsigned, int))REAL(XD_GetCurrentProtoAddr))(a, p); lg("XD_GetCurrentProtoAddr(%u,%d) = '%s'", a, p, r ? r : "(null)"); return r; }
char *w_XD_GetConnectedSocketAddr(void)
{ char *r = ((char *(*)(void))REAL(XD_GetConnectedSocketAddr))(); lg("XD_GetConnectedSocketAddr() = '%s'", r ? r : "(null)"); return r; }
int w_XD_GetXDaemonVersion(int *a, int *b, int *c)
{ int r = ((int (*)(int *, int *, int *))REAL(XD_GetXDaemonVersion))(a, b, c); lg("XD_GetXDaemonVersion = %d (%d.%d.%d)", r, *a, *b, *c); return r; }
int w_XD_InitializeSocketsTCP(int v, int port, int port2, int server, unsigned maxp, BcastCB cb)
{
    int r; g_init_cb = cb;
    r = ((int (*)(int, int, int, int, unsigned, BcastCB))REAL(XD_InitializeSocketsTCP))(v, port, port2, server, maxp, cb ? cb_initcb : NULL);
    lg("XD_InitializeSocketsTCP(v=%d, port=%d, port2=%d, server=%d, max=%u, cb=%p) = %d", v, port, port2, server, maxp, cb, r);
    return r;
}
int w_XD_OpenConnection(void *addr, unsigned long t)
{ int r; lg("XD_OpenConnection('%s', %lu) ...", (char *)addr, t); r = ((int (*)(void *, unsigned long))REAL(XD_OpenConnection))(addr, t); lg("XD_OpenConnection = %d", r); return r; }
int w_XD_ResetLibrary(void)
{ int r = ((int (*)(void))REAL(XD_ResetLibrary))(); lg("XD_ResetLibrary() = %d", r); return r; }
int w_XD_SendBroadcastData(void *m, unsigned long sz, long fl)
{ int r = ((int (*)(void *, unsigned long, long))REAL(XD_SendBroadcastData))(m, sz, fl); lg("XD_SendBroadcastData(size=%lu, flags=%ld) = %d", sz, fl, r); hex("tx", m, sz); return r; }
int w_XD_SendSecureData(unsigned short to, void *m, unsigned long sz, short fl)
{ int r = ((int (*)(unsigned short, void *, unsigned long, short))REAL(XD_SendSecureData))(to, m, sz, fl); lg("XD_SendSecureData(to=%u, size=%lu, flags=%d) = %d", to, sz, fl, r); hex("tx", m, sz); return r; }
int w_XD_SendUDPBroadcastData(void *m, unsigned long sz, long fl, const char *a, int b)
{ int r = ((int (*)(void *, unsigned long, long, const char *, int))REAL(XD_SendUDPBroadcastData))(m, sz, fl, a, b); lg("XD_SendUDPBroadcastData(size=%lu, %ld, '%s', %d) = %d", sz, fl, a ? a : "", b, r); hex("tx", m, sz); return r; }
int w_XD_SendDirectBroadcastData(void *m, void *a, unsigned long sz)
{ int r = ((int (*)(void *, void *, unsigned long))REAL(XD_SendDirectBroadcastData))(m, a, sz); lg("XD_SendDirectBroadcastData(size=%lu) = %d", sz, r); hex("tx", m, sz); return r; }
int w_XD_ServerCloseConnection(unsigned short id)
{ int r = ((int (*)(unsigned short))REAL(XD_ServerCloseConnection))(id); lg("XD_ServerCloseConnection(%u) = %d", id, r); return r; }
int w_XD_StopConnections(void)
{ int r = ((int (*)(void))REAL(XD_StopConnections))(); lg("XD_StopConnections() = %d", r); return r; }
void w_XD_ShutdownSockets(void)
{ lg("XD_ShutdownSockets()"); ((void (*)(void))REAL(XD_ShutdownSockets))(); }
int w_XD_EnableServerLocator(int a, unsigned b)
{ int r = ((int (*)(int, unsigned))REAL(XD_EnableServerLocator))(a, b); lg("XD_EnableServerLocator(%d, %u) = %d", a, b, r); return r; }
int w_XD_LaunchedByLobby(void *h, void *info)
{ int r = ((int (*)(void *, void *))REAL(XD_LaunchedByLobby))(h, info); lg("XD_LaunchedByLobby = %d", r); return r; }
int w_XD_LobbySendMessage(unsigned long m)
{ int r = ((int (*)(unsigned long))REAL(XD_LobbySendMessage))(m); lg("XD_LobbySendMessage(%lu) = %d", m, r); return r; }
void w_XD_SetBroadcastReceive(BcastCB cb)
{ g_bcast = cb; lg("XD_SetBroadcastReceive(%p)", cb); ((void (*)(BcastCB))REAL(XD_SetBroadcastReceive))(cb ? cb_bcast : NULL); }
void w_XD_SetSecureReceive(SecureCB cb)
{ g_secure = cb; lg("XD_SetSecureReceive(%p)", cb); ((void (*)(SecureCB))REAL(XD_SetSecureReceive))(cb ? cb_secure : NULL); }
void w_XD_SetNewClientConnection(NewClientCB cb)
{ g_newc = cb; lg("XD_SetNewClientConnection(%p)", cb); ((void (*)(NewClientCB))REAL(XD_SetNewClientConnection))(cb ? cb_newc : NULL); }
void w_XD_SetOnClientConnectionToServer(ConnCB cb)
{ g_conn = cb; lg("XD_SetOnClientConnectionToServer(%p)", cb); ((void (*)(ConnCB))REAL(XD_SetOnClientConnectionToServer))(cb ? cb_conn : NULL); }
void w_XD_SetOnConnectionLost(LostCB cb)
{ g_lost = cb; lg("XD_SetOnConnectionLost(%p)", cb); ((void (*)(LostCB))REAL(XD_SetOnConnectionLost))(cb ? cb_lost : NULL); }
int w_XD_SetOversizedMessageCB(unsigned long max, OversizeCB cb)
{ g_over = cb; lg("XD_SetOversizedMessageCB(%lu, %p)", max, cb); return ((int (*)(unsigned long, OversizeCB))REAL(XD_SetOversizedMessageCB))(max, cb ? cb_over : NULL); }
