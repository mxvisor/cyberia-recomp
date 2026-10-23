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

/* A directory in the game's data that tells a copy of the disc from an
 * installed game: the data directory is then the disc's root, the drive D:
 * (Game_cdrom.c).  The disc itself has no directories, so the marker is
 * empty and only a portable copy of the disc has it. */
#define GAME_CD_MARKER "_CD_"

/* The game runs from the disc: a path without a drive is looked up on the CD
 * first, then in the game directory (Game_gamedir.c). */
#define GAME_CD_CURRENT_DRIVE

/* The files the game directory must hold (Game_gamedir.c): the data of the
 * game is on the disc, and the port runs from a copy of it. */
#define GAME_DATA_MARKERS "JOY.DEF", "GLOBALS.DEF"

/* The game's own sound driver drives a Sound Blaster (BLASTER=A220 I5 D1 T1):
 * the runtime's card of Game_dossb.c, with its IRQ. */
#define GAME_DOS_SOUNDBLASTER

#define GAME_LOG_FILE "cyberia.log"

#endif /* _GAME_HOOKS_H_INCLUDED_ */
