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

/* The environment of the program, as the C library of the game reads it: the
 * array of the addresses of the variables, every one of them a "NAME=VALUE"
 * string, and a zero at the end of the array.
 *
 * The game is a DOS/4GW program, and the extender (4GW) is linked into it:
 * its getenv reads the array from its own variable in the data segment of the
 * program, and the array was built by the server of the extender, which is
 * not in the port (every INT 31h of the game is the runtime's).  The runtime
 * builds the array instead, and the recompiled code reads it where the
 * extender read its own copy (instruction_replacements.sci).
 *
 * The variables are the ones of the environment block of the conventional
 * memory, behind the PSP, which the game writes there (Game_runtime.c):
 * BLASTER among them, the game reads it to find the sound card.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "Game_hooks.h"
#include "Game_dosmem.h"
#include "Game_memory.h"
#include "Game_environ.h"

/* The array the code of the game reads.  It is in the memory of the process,
 * which is the memory of the guest as well (the guest's addresses are the
 * host's plus the pointer offset), so the code of the game can walk it. */
static uint32_t cyb_environ_array[16];

extern "C" {

/* The guest address of the array: what the code of the game loads.  The
 * recompiled code names it as a C symbol (game/llasm/extern.llinc:
 * "extern cyb_environ"), and the file is compiled as C++, where the name
 * would be mangled without this. */
uint32_t cyb_environ;

}

/* The rest of the file (the init) is C++ like the rest of the game. */

void Game_EnvironInit(void)
{
    const uint8_t *block = Game_DosMemBase();
    const char *string;
    uint32_t count = 0;

    if (block == NULL) {
        cyb_environ_array[0] = 0;
        cyb_environ = GAME_PTR_TO_GUEST32(cyb_environ_array);
        return;
    }

    /* the block of the environment: its first word is the segment of the
     * block, the second the number of the variables, the strings follow them,
     * and an empty string ends them */
    string = (const char *)(const void *)(block + CYB_ENV_OFFSET + 4u);
    while (string[0] != 0 &&
           count < (uint32_t)(sizeof(cyb_environ_array) / sizeof(cyb_environ_array[0])) - 1u) {
        cyb_environ_array[count] = GAME_PTR_TO_GUEST32((const uint8_t *)(const void *)string);
        count++;
        string += strlen(string) + 1u;
    }
    cyb_environ_array[count] = 0;

    cyb_environ = GAME_PTR_TO_GUEST32(cyb_environ_array);
}
