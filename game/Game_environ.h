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

#ifndef _GAME_ENVIRON_H_INCLUDED_
#define _GAME_ENVIRON_H_INCLUDED_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The environment block of the program: it is behind the PSP, in the
 * conventional memory (Game_runtime.c writes it there).  The DOS format: the
 * segment of the block, the number of the variables, the variables
 * ("NAME=VALUE"), a zero. */
#define CYB_ENV_OFFSET 0x100u

/* The environment of the program, as the C library of the game reads it: the
 * array of the addresses of the variables, with a zero at the end.  The
 * recompiled code reads it where the getenv of the extender (4GW) read its
 * own copy of the array (instruction_replacements.sci). */
void Game_EnvironInit(void);

#ifdef __cplusplus
}
#endif

#endif /* _GAME_ENVIRON_H_INCLUDED_ */
