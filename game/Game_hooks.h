/* What SR-SDK's runtime asks of this game.  Every hook is optional: see the
 * #ifndef defaults in SR-SDK/runtime/ for the full list.
 *
 * The game is a DOS/4GW program that starts at the entry point of the
 * extender, so the DOS world (the PSP, the environment, the VGA window, the
 * mouse) is the runtime's own. */

#ifndef _GAME_HOOKS_H_INCLUDED_
#define _GAME_HOOKS_H_INCLUDED_

/* the original's file name (argv[0] of the guest) */
#define GAME_MAIN_FILENAME "CYB.EXE"

/* the game's name in the port's messages */
#define GAME_NAME "cyberia"
/* the port's environment variables: GAME_ENV("CD_IMAGE") is $CYB_CD_IMAGE */
#define GAME_ENV(name) "CYB_" name

/* A file the game reads from the disc (D:GLOBALS.DEF; the install leaves it
 * there): the game directory holding it is then also the disc's root, the
 * drive D: (Game_cdrom.c), as in a distrib copy. */
#define GAME_CD_MARKER "GLOBALS.DEF"

/* The game runs from the disc: a path without a drive is looked up on the CD
 * first, then in the game directory (Game_gamedir.c). */
#define GAME_CD_CURRENT_DRIVE

/* The files the game directory must hold (Game_gamedir.c): JOY.DEF of the
 * install, or GLOBALS.DEF of the disc (the game's data stays on the disc). */
#define GAME_DATA_MARKERS "JOY.DEF", "GLOBALS.DEF"

/* The game's own sound driver drives a Sound Blaster (BLASTER=A220 I5 D1 T1):
 * the runtime's card of Game_dossb.c, with its IRQ. */
#define GAME_DOS_SOUNDBLASTER


#endif /* _GAME_HOOKS_H_INCLUDED_ */
