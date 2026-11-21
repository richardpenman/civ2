/*
 * The platform's part of the OpenCiv2 Options dialog (src/game/options.cpp,
 * docs/EXTRAS.md "OpenCiv2 Options"): the graphics' default and the
 * restart, for the choices that need one (not in a test run, a replay: the
 * program goes on).
 */
#include "plat.h"
#include "hd.h"
#include "../host/host.h"

/* ---- the graphics: the one place that decides (docs/EXTRAS.md "HD graphics") ----

   HD graphics where CIV.INI's [OpenCiv2] Graphics= says nothing: off (the
   original's look).  Making HD the default is this line (and its mirror in
   the browser's page, src/web/options.js: HD_BY_DEFAULT); Classic and
   Graphics=Standard keep the original's look where the game folder has its
   pictures. */
#define HD_BY_DEFAULT 0

/* The game folder lacks the original's pictures: HD graphics whatever is
   chosen, no Classic mode (set at the start, src/platform/main.c). */
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

int plat_restart(int now)
{
    if (plat_replay) {
        if (now)
            plat_log("restart: not in a test run\n");
        return now ? 0 : 1;
    }
    return host_restart(now);
}
