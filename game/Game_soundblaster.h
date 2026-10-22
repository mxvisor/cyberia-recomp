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

#ifndef _GAME_SOUNDBLASTER_H_INCLUDED_
#define _GAME_SOUNDBLASTER_H_INCLUDED_

#include <stdint.h>

#include "Game_dosregs.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The Sound Blaster the game's own driver is told about: the DSP at 220h,
 * IRQ 5, the 8-bit DMA channel 1.  The same as Ignition's card
 * (IGN-sb.h), and the same as the BLASTER of the original. */
#define CYB_SB_BASE 0x220u
#define CYB_SB_IRQ 5
#define CYB_SB_DMA 1
#define CYB_SB_BLASTER "BLASTER=A220 I5 D1 T1"

/* The card's ports: both return 1 when the port belongs to the card (the
 * value is the byte read), 0 when it does not.  The hooks of the game
 * (Game_hooks.h) call them from the runtime's I/O dispatch. */
int Game_DosPortIn(uint16_t port, uint32_t *value);
int Game_DosPortOut(uint16_t port, uint32_t value);

/* The card's IRQ, delivered from the game's own timer (Game_dostimer.c). */
void Game_SbDeliverIrq(void);

/* The mouse BIOS (INT 33h) the game asks for: the runtime's interrupt
 * dispatch calls it. */
int Game_DosInterrupt(uint8_t number, Game_DosRegs *regs);

#ifdef __cplusplus
}
#endif

#endif /* _GAME_SOUNDBLASTER_H_INCLUDED_ */
