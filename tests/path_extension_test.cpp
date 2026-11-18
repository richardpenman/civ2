// SOURCE src/game/regargs.cpp path_file_part
// SOURCE src/game/regargs.cpp path_default_extension
// SOURCE src/game/regargs.cpp path_set_extension
// A file's extension is looked for in its own name, not in its folder
// (docs/BUGS.md B39, issue #31): after a load from ~/.local/share/..., the
// game's GAME text file is "<folder>\GAME", and the dot of ".local" must not
// count as its extension.  Paths without a dot in a folder come out as before.
#include <cassert>
#include <cctype>
#include <cstring>
typedef char i8;  // (char with -fsigned-char, as in the game)
#define __cdecl
#define crt_strrchr strrchr
#define crt_strcat strcat
#define crt_strcpy strcpy
static i8 path_default_ext_dot_str[4] = {'.'};
static i8 path_set_ext_dot_str[4] = {'.'};
static void __strupr(char *s) {
    for (; *s; s++)
        *s = (char)toupper((unsigned char)*s);
}
#include "production.inc"

static void check_default(const char *in, const char *ext, const char *want) {
    char buf[260];
    strcpy(buf, in);
    path_default_extension(buf, (char *)ext);
    assert(strcmp(buf, want) == 0);
}

static void check_set(const char *in, const char *ext, const char *want) {
    char buf[260];
    path_set_extension(buf, (char *)in, (char *)ext);
    assert(strcmp(buf, want) == 0);
}

int main() {
    char path[] = "C:\\a.b\\c/d:e";
    assert(path_file_part(path) == path + 11);
    assert(strcmp(path_file_part((char *)"GAME"), "GAME") == 0);
    assert(*path_file_part((char *)"C:\\CIV2\\") == '\0');

    // as before: no dot in a folder
    check_default("GAME", "TXT", "GAME.TXT");
    check_default("C:\\CIV2\\GAME", "TXT", "C:\\CIV2\\GAME.TXT");
    check_default("C:\\CIV2\\GAME.TXT", "DE", "C:\\CIV2\\GAME.TXT");
    check_set("C:\\CIV2\\TEST.SCN", "TXT", "C:\\CIV2\\TEST.TXT");
    check_set("C:\\CIV2\\TEST", "TXT", "C:\\CIV2\\TEST.TXT");
    // a dot in a folder (issue #31)
    check_default("Z:\\home\\me\\.local\\share\\OpenCiv2\\GameData\\GAME", "TXT",
                  "Z:\\HOME\\ME\\.LOCAL\\SHARE\\OPENCIV2\\GAMEDATA\\GAME.TXT");
    check_default("C:\\CIV2\\SCENARIO\\..\\GAME", "TXT", "C:\\CIV2\\SCENARIO\\..\\GAME.TXT");
    check_default("/home/me/.local/GAME", "TXT", "/HOME/ME/.LOCAL/GAME.TXT");
    check_default("Z:\\home\\me\\.local\\GAME.SAV", "TXT", "Z:\\HOME\\ME\\.LOCAL\\GAME.SAV");
    check_set("Z:\\home\\me\\.local\\TEST", "TXT", "Z:\\HOME\\ME\\.LOCAL\\TEST.TXT");
    check_set("Z:\\home\\me\\.local\\TEST.SCN", "TXT", "Z:\\HOME\\ME\\.LOCAL\\TEST.TXT");
    return 0;
}
