/* What SR-SDK's runtime asks of this game.  Every hook is optional: see the
 * #ifndef defaults in SR-SDK/runtime/ for the full list.
 *
 * The same set as Ignition's port (IGNITION/ignition-recomp/game/Game_hooks.h):
 * the game brings its own DOS world (the PSP, the environment, the VGA
 * window), its own Sound Blaster and its own mouse. */

#ifndef _GAME_HOOKS_H_INCLUDED_
#define _GAME_HOOKS_H_INCLUDED_

/* the original's file name (argv[0] of the guest) */
#define GAME_MAIN_FILENAME "CYB.EXE"

/* the game's name in the port's messages */
#define GAME_NAME "cyberia"
/* the port's environment variables: GAME_ENV("CD_IMAGE") is $CYB_CD_IMAGE */
#define GAME_ENV(name) "CYB_" name

/* The game runs from the disc: a path without a drive is looked up on the CD
 * first, then in the game directory (Game_gamedir.c). */
#define GAME_CD_CURRENT_DRIVE

/* The files the game directory must hold (Game_gamedir.c): the data of the
 * game is on the disc, and the port runs from a copy of it. */
#define GAME_DATA_MARKERS "JOY.DEF", "GLOBALS.DEF"

/* The port starts at the entry point of the extender (the alias c_start_), not
 * at the C main: the program is a DOS/4GW one, and the extender's start-up
 * code (the stack and the heap in a block of its own) has to run. */
#define GAME_DOS_ENTRY

/* The DOS frame of the port (Game_dosmain.c): the game brings its own
 * conventional memory, its own environment and its own VGA window. */
#include "Game_runtime.h"
#define GAME_RUNTIME_INIT() Game_RuntimeInit()
#define GAME_RUNTIME_DEINIT() Game_RuntimeDeinit()
#define GAME_LOG_FILE "cyberia.log"

/* The game's own ports (Sound Blaster 0x220-0x22F, DMA 0x00-0x0F, 0x81-0x87),
 * reached from the runtime's I/O dispatch.  Both return 1 when the port
 * belongs to the card (value is the byte read), 0 when it does not. */
#include "Game_soundblaster.h"
#define GAME_DOS_PORT_IN(port, value) Game_DosPortIn(port, value)
#define GAME_DOS_PORT_OUT(port, value) Game_DosPortOut(port, value)

/* The game's own BIOS handlers (INT 33h mouse), reached from the runtime's
 * interrupt dispatch. */
#define GAME_DOS_INTERRUPT(number, regs) Game_DosInterrupt(number, regs)

#endif /* _GAME_HOOKS_H_INCLUDED_ */
