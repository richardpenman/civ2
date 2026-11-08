/*
 * Windows: the system's part of the set-up (setup_os.h).  The game is
 * found where its installer said (in the registry) or in the usual
 * folders, or its CD in a drive, or a disc image of the CD in the user's
 * folders, beside the program or at the top of a drive; GameData is beside
 * the program, with the
 * program's own files (the fonts, from its resources) in data.  The CD's
 * music is read from its drive (\\.\X:) with the CD-ROM ioctls.
 */
#include <windows.h>
#include <winioctl.h>
#include <ntddcdrm.h>
#include <shlobj.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include "setup.h"
#include "setup_os.h"

#define MGE_KEY "SOFTWARE\\MicroProse Software\\Civilization II Multiplayer Gold Edition"
#define TITLE "OpenCiv2"

int os_dir_exists(const char *p)
{
    DWORD a = GetFileAttributesA(p);
    return a != INVALID_FILE_ATTRIBUTES && (a & FILE_ATTRIBUTE_DIRECTORY);
}

int os_file_exists(const char *p)
{
    DWORD a = GetFileAttributesA(p);
    return a != INVALID_FILE_ATTRIBUTES && !(a & FILE_ATTRIBUTE_DIRECTORY);
}

void os_make_dirs(const char *path)
{
    char p[MAX_PATH];
    size_t i;
    snprintf(p, sizeof p, "%s", path);
    for (i = 3; p[i]; i++)
        if (p[i] == '\\' || p[i] == '/') {
            char c = p[i];
            p[i] = 0;
            CreateDirectoryA(p, NULL);
            p[i] = c;
        }
    CreateDirectoryA(p, NULL);
}

void os_remove_dir(const char *path)
{
    RemoveDirectoryA(path);
}

/* (names are found whatever their case) */
int os_entry(const char *dir, const char *name, char *out, size_t size)
{
    snprintf(out, size, "%s\\%s", dir, name);
    return GetFileAttributesA(out) != INVALID_FILE_ATTRIBUTES;
}

int os_each(const char *dir, int (*fn)(void *ctx, const char *name, int is_dir), void *ctx)
{
    WIN32_FIND_DATAA fd;
    char pat[MAX_PATH];
    HANDLE h;
    int ok = 1;
    snprintf(pat, sizeof pat, "%s\\*", dir);
    h = FindFirstFileA(pat, &fd);
    if (h == INVALID_HANDLE_VALUE)
        return 1;
    do
        if (strcmp(fd.cFileName, ".") && strcmp(fd.cFileName, ".."))
            ok = fn(ctx, fd.cFileName, (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
    while (ok && FindNextFileA(h, &fd));
    FindClose(h);
    return ok;
}

int os_copy_file(const char *from, const char *to)
{
    if (!CopyFileA(from, to, FALSE))
        return 0;
    SetFileAttributesA(to, FILE_ATTRIBUTE_NORMAL);     /* (read-only on the CD) */
    return 1;
}

int os_same_path(const char *a, const char *b)
{
    return !_stricmp(a, b);
}

void os_current_dir(char *out, size_t size)
{
    GetCurrentDirectoryA((DWORD)size, out);
}

/* The program's folder. */
static void program_dir(char *out, size_t size)
{
    char *slash;
    GetModuleFileNameA(NULL, out, (DWORD)size);
    if ((slash = strrchr(out, '\\')))
        *slash = 0;
}

void os_game_data(char *out, size_t size)
{
    char dir[MAX_PATH];
    program_dir(dir, sizeof dir);
    snprintf(out, size, "%s\\GameData", dir);
}

void os_message(const char *text, int kind)
{
    MessageBoxA(NULL, text, TITLE, MB_OK | (kind == OS_ERROR ? MB_ICONERROR : kind == OS_WARNING ? MB_ICONWARNING :
                                            MB_ICONINFORMATION));
}

/* ---- where the game is ---- */

static void registry_path(HKEY root, REGSAM view, char *out, size_t size)
{
    HKEY k;
    DWORD type, n = (DWORD)size - 1;
    out[0] = 0;
    if (RegOpenKeyExA(root, MGE_KEY, 0, KEY_READ | view, &k) != ERROR_SUCCESS)
        return;
    if (RegQueryValueExA(k, "Game Path", NULL, &type, (LPBYTE)out, &n) != ERROR_SUCCESS || type != REG_SZ)
        n = 0;
    out[n] = 0;
    RegCloseKey(k);
    while (n > 3 && (out[n - 1] == '\\' || out[n - 1] == '/' || out[n - 1] == 0))
        out[--n] = 0;
}

/* The installed game: where the registry says, else the usual folders. */
int os_find_installed(char *out, size_t size)
{
    static const HKEY roots[] = {HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER};
    static const REGSAM views[] = {KEY_WOW64_32KEY, KEY_WOW64_64KEY};
    static const char *const bases[] = {"ProgramFiles(x86)", "ProgramFiles", "ProgramW6432"};
    static const char *const folders[] = {"MicroProse Software\\Civilization II Multiplayer Gold Edition",
                                          "MicroProse\\Civilization II Multiplayer Gold Edition"};
    static const char *const others[] = {"C:\\MPS\\CIV2", "C:\\Civ2", "C:\\Games\\Civ2"};
    const char *v;
    size_t i, k;
    for (i = 0; i < sizeof roots / sizeof roots[0]; i++)
        for (k = 0; k < sizeof views / sizeof views[0]; k++) {
            registry_path(roots[i], views[k], out, size);
            if (out[0] && setup_game_folder(out))
                return 1;
        }
    for (i = 0; i < sizeof bases / sizeof bases[0]; i++)
        if ((v = getenv(bases[i])) && *v)
            for (k = 0; k < sizeof folders / sizeof folders[0]; k++) {
                snprintf(out, size, "%s\\%s", v, folders[k]);
                if (setup_game_folder(out))
                    return 1;
            }
    for (i = 0; i < sizeof others / sizeof others[0]; i++) {
        snprintf(out, size, "%s", others[i]);
        if (setup_game_folder(out))
            return 1;
    }
    if ((v = getenv("USERPROFILE")) && *v) {
        snprintf(out, size, "%s\\Civ2", v);
        if (setup_game_folder(out))
            return 1;
    }
    return 0;
}

/* The CD, in any CD drive (an empty drive passed over without asking). */
int os_find_cd(char *out, size_t size)
{
    UINT old = SetErrorMode(SEM_FAILCRITICALERRORS);
    DWORD drives = GetLogicalDrives();
    char root[4] = "A:\\";
    int i, found = 0;
    for (i = 0; i < 26 && !found; i++)
        if (drives & (1u << i)) {
            root[0] = (char)('A' + i);
            if (GetDriveTypeA(root) == DRIVE_CDROM) {
                snprintf(out, size, "%c:", 'A' + i);
                found = setup_game_cd(out);
            }
        }
    SetErrorMode(old);
    return found;
}

/* A disc image of the CD: in the user's folders (setup_image_in_home, and
   the Desktop where Windows has moved it), beside the program (or in a
   folder there), at the top of a drive other than a floppy or the
   network's. */
int os_find_image(char *out, size_t size)
{
    char dir[MAX_PATH];
    const char *v;
    UINT old;
    DWORD drives;
    int i, found = 0;
    if ((v = getenv("USERPROFILE")) && *v && setup_image_in_home(v, out, size))
        return 1;
    if (SHGetFolderPathA(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, dir) == S_OK && setup_image_in(dir, 1, out, size))
        return 1;
    program_dir(dir, sizeof dir);
    if (setup_image_in(dir, 1, out, size))
        return 1;
    old = SetErrorMode(SEM_FAILCRITICALERRORS);
    drives = GetLogicalDrives();
    for (i = 2; i < 26 && !found; i++)
        if (drives & (1u << i)) {
            UINT type;
            snprintf(dir, sizeof dir, "%c:\\", 'A' + i);
            type = GetDriveTypeA(dir);
            if (type == DRIVE_FIXED || type == DRIVE_REMOVABLE || type == DRIVE_CDROM) {
                dir[2] = 0;
                found = setup_image_in(dir, 0, out, size);
            }
        }
    SetErrorMode(old);
    return found;
}

/* The user's choice of folder: the installed game or the CD, or a disc
   image (its .cue: the dialog shows files too).  0: none. */
int os_ask_folder(char *out, size_t size)
{
    BROWSEINFOA bi;
    LPITEMIDLIST id;
    char path[MAX_PATH];
    int ok = 0;
    memset(&bi, 0, sizeof bi);
    bi.lpszTitle = "Where is Civilization II Multiplayer Gold Edition? Choose the folder it is "
                   "installed in, its CD, or a disc image of the CD (its .cue file, or the folder it is in).";
    bi.ulFlags = BIF_NEWDIALOGSTYLE | BIF_BROWSEINCLUDEFILES;     /* (a path: SHGetPathFromIDList) */
    CoInitialize(NULL);
    id = SHBrowseForFolderA(&bi);
    if (id) {
        ok = SHGetPathFromIDListA(id, path);
        CoTaskMemFree(id);
    }
    CoUninitialize();
    if (ok) {
        size_t n = strlen(path);
        while (n > 2 && (path[n - 1] == '\\' || path[n - 1] == '/'))
            path[--n] = 0;
        snprintf(out, size, "%s", path);
    }
    return ok;
}

/* ---- the progress window ---- */

static HWND progress, progress_text;
static int cancelled;

static LRESULT CALLBACK progress_proc(HWND h, UINT msg, WPARAM wp, LPARAM lp)
{
    if (msg == WM_CLOSE) {
        cancelled = 1;
        return 0;
    }
    return DefWindowProcA(h, msg, wp, lp);
}

void os_progress_open(void)
{
    WNDCLASSA wc;
    RECT r;
    memset(&wc, 0, sizeof wc);
    wc.lpfnWndProc = progress_proc;
    wc.hInstance = GetModuleHandleA(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_WAIT);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = "OpenCiv2Setup";
    RegisterClassA(&wc);
    SystemParametersInfoA(SPI_GETWORKAREA, 0, &r, 0);
    progress = CreateWindowA(wc.lpszClassName, TITLE ": setting up the game", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
                             (r.left + r.right) / 2 - 260, (r.top + r.bottom) / 2 - 60, 520, 120, NULL, NULL,
                             wc.hInstance, NULL);
    progress_text = CreateWindowA("STATIC", "", WS_CHILD | WS_VISIBLE | SS_LEFT | SS_PATHELLIPSIS, 16, 16, 480, 50,
                                  progress, NULL, wc.hInstance, NULL);
    SendMessageA(progress_text, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
    ShowWindow(progress, SW_SHOW);
}

int os_progress_show(const char *what, const char *detail)
{
    char text[MAX_PATH + 128];
    MSG m;
    snprintf(text, sizeof text, "%s\n%s", what, detail ? detail : "");
    if (progress_text)
        SetWindowTextA(progress_text, text);
    while (PeekMessageA(&m, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&m);
        DispatchMessageA(&m);
    }
    return cancelled;
}

void os_progress_close(void)
{
    if (progress)
        DestroyWindow(progress);
    progress = progress_text = NULL;
}

/* ---- the program's own files ---- */

/* Resource id's file into path: 8 bytes of size, then the file deflated
   (tools/build_port.py). */
static int unpack_resource(int id, const char *path)
{
    HRSRC r = FindResourceA(NULL, MAKEINTRESOURCEA(id), (LPCSTR)RT_RCDATA);
    HGLOBAL g = r ? LoadResource(NULL, r) : NULL;
    const unsigned char *data = g ? LockResource(g) : NULL;
    DWORD n = r ? SizeofResource(NULL, r) : 0;
    unsigned char out[1 << 16];
    z_stream z;
    FILE *f;
    int st = Z_OK;
    if (!data || n < 8 || !(f = fopen(path, "wb")))
        return 0;
    memset(&z, 0, sizeof z);
    inflateInit(&z);
    z.next_in = (unsigned char *)data + 8;
    z.avail_in = n - 8;
    while (st == Z_OK) {
        z.next_out = out;
        z.avail_out = sizeof out;
        st = inflate(&z, Z_NO_FLUSH);
        if (st != Z_OK && st != Z_STREAM_END)
            break;
        fwrite(out, 1, sizeof out - z.avail_out, f);
    }
    inflateEnd(&z);
    if (fclose(f) != 0 || st != Z_STREAM_END) {
        DeleteFileA(path);
        return 0;
    }
    return 1;
}

/* Validate the extracted file against the embedded zlib stream's checksum so
   replacing the executable also updates artwork (and repairs interrupted writes). */
static int resource_matches(int id,const char *path)
{
    HRSRC r=FindResourceA(NULL,MAKEINTRESOURCEA(id),(LPCSTR)RT_RCDATA);
    HGLOBAL g=r?LoadResource(NULL,r):NULL;
    const unsigned char *data=g?LockResource(g):NULL;
    DWORD n=r?SizeofResource(NULL,r):0;
    unsigned char buf[65536];size_t got;uLong sum=adler32(0L,Z_NULL,0);FILE *f;
    unsigned long long size=0,expected=0;int i;
    if(!data || n<12 || !(f=fopen(path,"rb")))return 0;
    for(i=0;i<8;i++)expected|=(unsigned long long)data[i]<<(8*i);
    while((got=fread(buf,1,sizeof buf,f))!=0){size+=got;sum=adler32(sum,buf,(uInt)got);}
    i=!ferror(f);fclose(f);
    return i && size==expected && sum==((uLong)data[n-4]<<24 | (uLong)data[n-3]<<16 | (uLong)data[n-2]<<8 | data[n-1]);
}

/* The program's own fonts and replacement graphics beside the executable. */
int os_program_files(void)
{
    static const struct { int id; const char *name; } files[] = {
        {SETUP_RES_FONTS, "fonts.bin"}, {SETUP_RES_FONTS_AA, "fonts_aa.bin"}, {SETUP_RES_ATLAS,"atlas-hd.pack"}};
    char dir[MAX_PATH], data[MAX_PATH], path[MAX_PATH];
    size_t i;
    program_dir(dir, sizeof dir);
    snprintf(data, sizeof data, "%s\\data", dir);
    for (i = 0; i < sizeof files / sizeof files[0]; i++) {
        snprintf(path, sizeof path, "%s\\%s", data, files[i].name);
        if (resource_matches(files[i].id,path))
            continue;
        os_make_dirs(data);
        os_progress_show("Unpacking", path);
        if (!unpack_resource(files[i].id, path)) {
            char text[MAX_PATH + 64];
            snprintf(text, sizeof text, "Could not write %s.", path);
            os_message(text, OS_ERROR);
            return 0;
        }
    }
    return 1;
}

int os_progress_stopped(void)
{
    return cancelled;
}

/* (the progress window shows what is set up, and can stop it) */
int os_confirm(const char *text)
{
    (void)text;
    return 1;
}

/* ---- the CD's music, from its drive ---- */

#define CD_SECTOR 2352
#define CD_CHUNK 16

struct cd {
    HANDLE h;
    unsigned char *buf;         /* (page-aligned, as some drives want) */
};

int os_cd_open(const char *cd, struct cd_toc *toc, void **h)
{
    char root[4] = "A:\\", dev[8] = "\\\\.\\A:";
    CDROM_TOC t;
    DWORD n;
    struct cd *c;
    HANDLE f;
    int i, count;
    UINT type;
    if (!((cd[0] >= 'A' && cd[0] <= 'Z') || (cd[0] >= 'a' && cd[0] <= 'z')) || cd[1] != ':')
        return 0;
    root[0] = dev[4] = cd[0];
    type = GetDriveTypeA(root);
    if (type != DRIVE_CDROM)
        return 0;
    f = CreateFileA(dev, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (f == INVALID_HANDLE_VALUE)
        return -1;
    memset(&t, 0, sizeof t);
    count = 0;
    if (!DeviceIoControl(f, IOCTL_CDROM_READ_TOC, NULL, 0, &t, sizeof t, &n, NULL) || t.LastTrack < t.FirstTrack ||
        (count = t.LastTrack - t.FirstTrack + 1) >= MAXIMUM_NUMBER_TRACKS) {
        CloseHandle(f);
        return -1;
    }
    /* (each track's start as MSF, Address[1..3], 150 sectors before
       sector 0; after the last, the lead-out) */
    memset(toc, 0, sizeof *toc);
    for (i = 0; i <= count && i < 99; i++) {
        const TRACK_DATA *d = &t.TrackData[i];
        long lba = ((long)d->Address[1] * 60 + d->Address[2]) * 75 + d->Address[3] - 150;
        if (i == count) {
            toc->leadout = lba;
        } else {
            toc->t[i].number = d->TrackNumber;
            toc->t[i].audio = !(d->Control & 4);
            toc->t[i].lba = lba;
            toc->ntracks++;
        }
    }
    c = malloc(sizeof *c);
    if (!c || !(c->buf = VirtualAlloc(NULL, CD_CHUNK * CD_SECTOR, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE))) {
        free(c);
        CloseHandle(f);
        return -1;
    }
    c->h = f;
    *h = c;
    return 1;
}

int os_cd_read(void *h, long lba, int n, unsigned char *buf)
{
    struct cd *c = h;
    RAW_READ_INFO r;
    DWORD got = 0;
    if (n < 1 || n > CD_CHUNK)
        return 0;
    memset(&r, 0, sizeof r);
    r.DiskOffset.QuadPart = (LONGLONG)lba * 2048;     /* (in 2048-byte sectors, though raw ones are read) */
    r.SectorCount = (ULONG)n;
    r.TrackMode = CDDA;
    if (!DeviceIoControl(c->h, IOCTL_CDROM_RAW_READ, &r, sizeof r, c->buf, (DWORD)n * CD_SECTOR, &got, NULL) ||
        got != (DWORD)n * CD_SECTOR)
        return 0;
    memcpy(buf, c->buf, (size_t)n * CD_SECTOR);
    return 1;
}

void os_cd_close(void *h)
{
    struct cd *c = h;
    CloseHandle(c->h);
    VirtualFree(c->buf, 0, MEM_RELEASE);
    free(c);
}
