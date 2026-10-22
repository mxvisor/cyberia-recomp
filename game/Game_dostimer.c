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

/* The timer of the game: the default one of the runtime (Game_dostimer.c) and
 * the interrupt of the sound card.
 *
 * The translated code calls SR_CheckTimer from every ACTION_* macro (at the
 * backward jumps and the calls), so it is where the host gets a turn: the
 * keyboard is drained, the PIT's channel 0 interrupt (INT 8) is due, and the
 * screen is presented about 30 times a second.  The sound card of the game
 * (Game_soundblaster.c) has to raise its IRQ when the block of the DMA
 * transfer wraps, and this is the place where that happens often enough:
 * the runtime's own SR_CheckTimer does not know about the card.
 *
 * A game that has to do more at INT 8 defines its own SR_CheckTimer and lists
 * Game_dostimer.c in runtime_skip (game.cfg), the way MD-runtime.c does.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>

#include "Game_hooks.h"
#include "Game_vars.h"
#include "Game_thread.h"
#include "Game_dosint.h"
#include "Game_doskbd.h"
#include "Game_dospit.h"
#include "Game_dosrt.h"
#include "Game_soundblaster.h"
#include "Game_vars.h"

/* CYB_TRACE_TIMER: the ticks of the PIT, to see that the timer of the game
 * runs (the game's tasks wait for them). */
static int trace_timer(void)
{
    static int enabled = -1;
    if (enabled < 0) enabled = getenv(GAME_ENV("TRACE_TIMER")) != NULL;
    return enabled;
}

static void pit_irq(void)
{
    static unsigned int ticks;

    Game_DosTimerTick();
    ticks++;
    if (trace_timer() && (ticks % 100u) == 0u) {
        fprintf(stderr, "TIMER %u ticks, vector 08=%08x 1c=%08x, if=%u\n", ticks,
                Game_DosGetVector(0x08), Game_DosGetVector(0x1c), X86_InterruptFlag);
    }
    if (Thread_Exit) Game_StopMain();
}

extern "C" void SR_CheckTimer(void)
{
    static uint32_t last_present;
    uint32_t now;

    if (Thread_Exit) Game_StopMain();

    Game_DosKbdDrain();
    now = SDL_GetTicks();

    Game_DosPitRun(pit_irq);

    Game_DosKbdDeliverIrq();

    /* the sound card: its IRQ at the end of a block of the DMA transfer */
    Game_SbDeliverIrq();

    if (now - last_present >= 33)
    {
        Game_DosRequestPresent();
        last_present = now;
    }
}
