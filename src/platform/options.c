/*
 * The platform's part of the OpenCiv2 Options dialog (src/game/options.cpp,
 * docs/EXTRAS.md "OpenCiv2 Options"): the HD graphics' art (whether it is
 * there, its download: src/host/host_options.c) and the restart, for the
 * choices that need one.  Neither in a test run (a replay): nothing is
 * downloaded and the program goes on.
 */
#include "plat.h"
#include "hd.h"
#include "../host/host.h"

/* (the game's names for the host's states, src/include/extras.h) */
_Static_assert(PLAT_HD_POLL == HOST_HD_POLL && PLAT_HD_CANCEL == HOST_HD_CANCEL && PLAT_HD_LATER == HOST_HD_LATER &&
               PLAT_HD_IDLE == HOST_HD_IDLE && PLAT_HD_DONE == HOST_HD_DONE, "the HD art's states");

int plat_hd_art(int op, int *info, char *msg, int n)
{
    char path[1100];
    int32_t in[3] = {0, 0, 0};
    int r;
    if (msg && n > 0)
        msg[0] = 0;
    if (plat_replay) {
        /* (tests: the art never there, its size the pinned one's) */
        if (info) {
            info[0] = info[1] = 0;
            info[2] = 228595;
        }
        if (op == HOST_HD_START && msg && n > 0)
            snprintf(msg, (size_t)n, "@hd.err.test_run");
        return op == HOST_HD_START ? HOST_HD_FAILED : HOST_HD_IDLE;
    }
    if (op == HOST_HD_POLL && hd_pack_path(path, sizeof path)) {
        r = host_hd_fetch(op, in, NULL, 0);
        if (r != HOST_HD_RUNNING) {
            if (info)
                info[0] = info[1] = 0, info[2] = in[2];
            return HOST_HD_DONE;
        }
    }
    r = host_hd_fetch(op, in, msg, n);
    if (info) {
        info[0] = in[0];
        info[1] = in[1];
        info[2] = in[2];
    }
    return r;
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
