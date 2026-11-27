/*
 * The platform's part of the Settings dialog (src/game/options.cpp,
 * docs/EXTRAS.md "Settings"): the graphics' default and the
 * restart, for the choices that need one (not in a test run, a replay: the
 * program goes on).
 */
#include "plat.h"
#include "hd.h"
#include "../host/host.h"

/* ---- the graphics: the one place that decides (docs/EXTRAS.md "HD graphics") ----

   HD graphics where CIV.INI's [OpenCiv2] Graphics= says nothing: on
   (Settings' "Classic graphics" unticked).  Making the original's look the
   default again is this line (and its mirror in the browser's page,
   src/web/options.js: HD_BY_DEFAULT).  Graphics=Classic (or Standard, as
   earlier versions wrote it) keeps the original's look where the game
   folder has its pictures; without the HD art (a build with HD_ART=) the
   game has the original's look whatever is chosen (hd_init).  Neither
   CIV2_CLASSIC=1 nor a replay (unless CIV2_TEST_HD) asks here. */
#define HD_BY_DEFAULT 1

/* The game folder lacks the original's pictures: HD graphics whatever is
   chosen (set at the start, src/platform/main.c). */
int plat_hd_forced;

/* Whether Graphics=value asks for HD graphics (value "" or NULL: none set). */
int plat_graphics_hd(const char *value)
{
    return value && *value ? !strcasecmp(value, "HD") : HD_BY_DEFAULT;
}

int plat_graphics_forced(void)
{
    return plat_hd_forced;
}

/* The original's files from the player's copy (Settings' Classic
   graphics ticked without them): the host's set-up (host_add_original),
   then the game folder looked at again. */
int plat_add_original(void)
{
    const char *missing;
    int r;
    if (plat_replay) {
        plat_log("add original: not in a test run\n");
        return -1;
    }
    r = host_add_original();
    if (r <= 0)
        return r;
    missing = synth_missing();
    if (missing)
        plat_log("add original: the game folder still has no %s\n", missing);
    return !missing;
}

int plat_restart(int now)
{
    if (plat_replay) {
        if (now)
            plat_log("restart: not in a test run\n");
        return now ? 0 : 1;
    }
    return host_restart(now);
}
