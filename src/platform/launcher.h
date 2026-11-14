/*
 * The opening window (src/platform/launcher.c): shown before the game
 * starts, where the player chooses the mode, HD graphics, the rule changes
 * and the interface additions, kept in CIV.INI's [OpenCiv2] section.
 */
#ifndef CIV2_LAUNCHER_H
#define CIV2_LAUNCHER_H
#ifdef __cplusplus
extern "C" {
#endif

/* Whether to show it now: Launcher= not 0 in CIV.INI, --launcher
   (CIV2_LAUNCHER_ARG), Shift held down; never in replays (but with
   CIV2_TEST_LAUNCHER=1) or in the browser. */
int launcher_wanted(void);
/* Its size, in desktop pixels: the desktop while it is shown. */
void launcher_size(int *w, int *h);
/* Shown until Play (1: the choices written) or Quit (0). */
int launcher_run(void);
/* A setting chosen in it in a test run (CIV2_TEST_LAUNCHER), which the
   game reads instead of CIV.INI: 1 and its text in out. */
int launcher_setting(const char *key, char *out, int n);

#ifdef __cplusplus
}
#endif
#endif
