// SOURCE src/platform/filedlg.c file_preselect
// The file dialog selects the proposed file name when it opens, as the
// original game's (Windows' own dialog) does, so that typing replaces it:
// FilePreselect in CIV.INI's [OpenCiv2], 1 when it is not set (the original's
// behaviour, in every mode, as FileSort and FileFolder are read); 0 leaves the
// name unselected, the caret at its start.
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

static const char *ini_value;   // [OpenCiv2] FilePreselect=, or none
static void setting_get(const char *key, char *out, int n)
{
    out[0] = 0;
    if (!strcmp(key, "FilePreselect") && ini_value)
        snprintf(out, (size_t)n, "%s", ini_value);
}

#include "production.inc"

int main()
{
    ini_value = nullptr;
    std::printf("not set: %d\n", file_preselect());
    assert(file_preselect() == 1);
    ini_value = "";
    assert(file_preselect() == 1);
    ini_value = "1";
    assert(file_preselect() == 1);
    ini_value = "0";
    assert(file_preselect() == 0);
    return 0;
}
