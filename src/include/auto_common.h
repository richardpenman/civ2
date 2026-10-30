/*
 * Common prelude for the machine-translated tier (src/auto/).
 */
#ifndef CIV2_AUTO_COMMON_H
#define CIV2_AUTO_COMMON_H

/* Native builds: the Windows API is provided by src/platform/. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../platform/win32_api.h"

#include "ghidra.h"
/* The code was written for MSVC, whose `long` is 32-bit (the system headers
 * above keep their own meaning). */
#define long int
#include "civ2types.h"
#include "civ2classes.h"
#include "enums.h"
#include "../data/gamedata.h"

/* new T[n] (MSVC): the element count is kept in the 4 bytes before the
 * array (vector deleting destructors read it back). */
#define VECTOR_COUNT(array) (((u32 *)(array))[-1])

/* Ghidra refers to Win32 structs by their tag names. */
typedef struct tagRECT tagRECT;
typedef struct tagPOINT tagPOINT;
typedef struct tagSIZE tagSIZE;
typedef struct tagMSG tagMSG;
typedef struct tagPAINTSTRUCT tagPAINTSTRUCT;
typedef struct tagTEXTMETRICA tagTEXTMETRICA;
typedef struct tagWNDCLASSA tagWNDCLASSA;
typedef struct tagLOGFONTA tagLOGFONTA;
typedef struct tagBITMAPINFO tagBITMAPINFO;
typedef struct tagBITMAPINFOHEADER tagBITMAPINFOHEADER;
typedef struct tagRGBQUAD tagRGBQUAD;
typedef struct tagPALETTEENTRY tagPALETTEENTRY;
typedef struct tagLOGPALETTE tagLOGPALETTE;
typedef struct tagOFNA tagOFNA;
typedef struct tagWINDOWPOS tagWINDOWPOS;
typedef struct tagMINMAXINFO tagMINMAXINFO;
typedef struct tagCREATESTRUCTA tagCREATESTRUCTA;
typedef struct tagMENUITEMINFOA tagMENUITEMINFOA;
typedef struct _MMIOINFO _MMIOINFO;
typedef struct _MMCKINFO _MMCKINFO;
typedef struct _OFSTRUCT _OFSTRUCT;
typedef struct _SYSTEMTIME _SYSTEMTIME;
typedef struct _WIN32_FIND_DATAA _WIN32_FIND_DATAA;
typedef struct _EXCEPTION_POINTERS _EXCEPTION_POINTERS;
typedef struct _CONTEXT _CONTEXT;
typedef struct mmtime_tag mmtime_tag;
#include "xdaemon.h"
#include "protos.h"
/* Calls through incremental-link thunks go straight to their targets. */
#include "thunks.h"
/* Scope-bound release: files, DCs, locks, local objects (step 6, RAII). */
#include "scoped.h"
/* The original's bugs (docs/BUGS.md) are fixed unless the original game is
 * wanted: CIV2_CLASSIC=1, replays and unit tests (plat_fix_bugs,
 * src/platform/main.c).  Each fix: if (bugfix_state()) <fixed> else
 * <original>, marked B<n>.  bugfix_state: a fix that changes the game's state,
 * also off in network games (every player, the original game included, must
 * compute the same game); bugfix_ui: one that changes only what this player
 * sees. */
EXTERN_C int plat_fix_bugs;
static inline int bugfix_ui(void) { return plat_fix_bugs; }
static inline int bugfix_state(void) { return plat_fix_bugs && game.multiplayer < 3; }
/* B32: a rules editor's new name written over a name in the string pool, at
 * the editor's width (_strncpy pads it with NULs).  A pool entry can be longer
 * than that width (a scenario's RULES.TXT); a shorter new name then leaves the
 * end of the old one after the NULs, which label_string counts as an entry of
 * its own: every later name moves up by one.  Fixed: the rest of the old name
 * is cleared too. */
static inline void editor_copy_pool_name(char *dest, const char *src, size_t width)
{
  size_t old_len = bugfix_state() ? _strlen(dest) : 0;
  _strncpy(dest, src, width);
  if (old_len > width)
    memset(dest + width, 0, old_len - width);
}
/* The interface additions (docs/EXTRAS.md): extra_ui(X_...), marked X<n>. */
#include "extras.h"
/* The sounds played and videos looked for, logged in a replay for the
 * comparison with the original (CIV2_CALL_LOG, src/platform/replay.c). */
EXTERN_C void replay_log_sound(int effect, int mode, int loop);
EXTERN_C void replay_log_video(const char *path);
/* rand() calls name their caller for the rand trace (src/native/crt.cpp). */
int crt_rand_at(const char *caller);
#define _rand() crt_rand_at(__func__)

#endif
