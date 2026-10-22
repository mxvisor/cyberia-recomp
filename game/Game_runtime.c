/**
 *
 *  Copyright (C) 2026 mxvisor
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of
 *  this software and associated documentation files (the "Software"), to deal in
 *  the Software without restriction, including without limitation the rights to
 *  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 *  of the Software, and to permit persons to whom the Software is furnished to do
 *  so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 */

/* What the DOS runtime of SR-SDK (runtime/dos) leaves to this game: the
 * conventional memory with its PSP and its environment, the VGA window, the
 * disc and the music.
 *
 * The game is a DOS/4GW program: the extender (4GW) is linked into it and
 * starts at its own entry point (c_start_), the stack and the heap in a block
 * of its own.  The environment of the program is the block behind the PSP,
 * and the C library of the game reads it with the getenv of the extender.
 * The block holds the settings of the sound card the game's own driver looks
 * for (BLASTER): without it the driver says the card is not there and waits
 * for an ENTER.
 *
 * Ignition's port does the same (IGN-runtime.c), and MD-runtime.c as well.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "Game_defs.h"
#include "Game_hooks.h"
#include "Game_vars.h"
#include "Game_memory.h"
#include "Game_cdrom.h"
#include "Game_music.h"
#include "Game_dosmem.h"
#include "Game_dosrt.h"
#include "Game_vfs.h"
#include "Game_runtime.h"
#include "Game_environ.h"
#include "Game_soundblaster.h"

/* The conventional memory: the PSP at 0, the environment behind it, the
 * arena for the blocks the extender asks for above them. */
#define CYB_LOWMEM_BASE 0x00000u
#define CYB_LOWMEM_SIZE 0xa0000u
#define CYB_ENV_OFFSET  0x100u
#define CYB_ENV_SIZE    0x100u
#define CYB_ARENA_START (CYB_ENV_OFFSET + CYB_ENV_SIZE)

/* The VGA window: the guest's A0000h, 64 KiB of it. */
#define CYB_VGA_BASE 0xa0000u
#define CYB_VGA_SIZE 0x10000u

int Game_RuntimeInit(void)
{
    uint8_t *psp, *env;
    uint16_t env_segment;

    if (Game_DosRuntimeInit(CYB_LOWMEM_BASE, CYB_LOWMEM_SIZE, CYB_ARENA_START) != 0) return -1;
    psp = Game_DosMemBase();

    /* the PSP: INT 20h, the end of the memory, the environment's segment */
    psp[0] = 0xcd;
    psp[1] = 0x20;
    psp[2] = (uint8_t)(CYB_LOWMEM_SIZE >> 12);
    psp[3] = (uint8_t)(CYB_LOWMEM_SIZE >> 20);
    env_segment = (uint16_t)((CYB_LOWMEM_BASE + CYB_ENV_OFFSET) >> 4);
    psp[0x2c] = (uint8_t)(env_segment & 0xffu);
    psp[0x2d] = (uint8_t)(env_segment >> 8);

    /* the environment: the card the game's sound driver looks for, then the
     * end of the block (the DOS format: the segment, the number of the
     * variables, the variables, a zero) */
    env = psp + CYB_ENV_OFFSET;
    env[2] = 1;                          /* one variable */
    env[3] = 0;
    memcpy(env + 4, CYB_SB_BLASTER, sizeof(CYB_SB_BLASTER));
    env[4 + sizeof(CYB_SB_BLASTER) - 1] = 0;

    /* the VGA window: the guest's A0000h */
    Game_FrameBuffer = Game_DosMapLow(CYB_VGA_BASE, CYB_VGA_SIZE);
    if (Game_FrameBuffer == NULL) {
        fprintf(stderr, GAME_NAME ": no memory for the VGA window\n");
        return -1;
    }
    memset(Game_FrameBuffer, 0, CYB_VGA_SIZE);
    Game_ScreenWindow = Game_FrameBuffer;
    Game_ScreenWindowNum = 0;

    /* the environment as the C library of the game reads it */
    Game_EnvironInit();

    /* the disc (the game's data files) and the music */
    Game_CdInit(Game_DataRoot());
    Game_MusicInit();
    return 0;
}

void Game_RuntimeDeinit(void)
{
    Game_MusicDeinit();
    Game_CdDeinit();
    Game_DosRuntimeDeinit();
}
