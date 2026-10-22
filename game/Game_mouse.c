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

/* The mouse of the game.
 *
 * The game does not talk to the mouse BIOS of INT 33h (the function in AH),
 * but to the Microsoft mouse driver: the function is in AX, the arguments in
 * BX, CX, DX.  Its calls are visible in the recompiled code: AX=0000h (the
 * driver is there), AX=0003h (the position and the buttons), AX=0004h (set
 * the position), AX=0007h/0008h (the smallest and the largest X and Y) and
 * AX=001Ch (the user's own handler, which the game leaves at 0).
 *
 * The driver counts the X in half steps: the game halves what it gets
 * ("shr cx, 1"), so the position is reported doubled.
 *
 * The position comes from SDL and is mapped into the screen of the game (the
 * 320x200 of mode 13h): the window shows the picture in the largest 4:3
 * rectangle (Game_dosdisplay.c), and the mouse is in the window's
 * coordinates.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>

#include "Game_defs.h"
#include "Game_dosregs.h"
#include "Game_hooks.h"
#include "Game_vars.h"
#include "Game_mouse.h"

/* the screen of the game: mode 13h */
#define MOUSE_WIDTH 320
#define MOUSE_HEIGHT 200

/* the position in the screen of the game */
static int mouse_x = MOUSE_WIDTH / 2;
static int mouse_y = MOUSE_HEIGHT / 2;
static int mouse_min_x;
static int mouse_max_x = MOUSE_WIDTH - 1;
static int mouse_min_y;
static int mouse_max_y = MOUSE_HEIGHT - 1;
/* the user's own handler (AX=001Ch); 0: the driver's own */
static uint32_t mouse_handler;

static int trace_mouse(void)
{
    static int enabled = -1;
    if (enabled < 0) enabled = getenv(GAME_ENV("TRACE_MOUSE")) != NULL;
    return enabled;
}

/* The mouse of the window, in the screen of the game. */
static void mouse_read(int *x, int *y, int *buttons)
{
    int window_x = 0, window_y = 0;
    int width = (int)Display_Width;
    int height = (int)Display_Height;
    int picture_w, picture_h, picture_x, picture_y;
    Uint32 state;

    state = SDL_GetMouseState(&window_x, &window_y);
    if (width <= 0 || height <= 0) {
        width = MOUSE_WIDTH;
        height = MOUSE_HEIGHT;
    }
    /* the picture in the window: the largest 4:3 rectangle, centred (the same
     * as the display's Display_PictureRect) */
    picture_w = width;
    picture_h = width * 3 / 4;
    if (picture_h > height) {
        picture_h = height;
        picture_w = height * 4 / 3;
    }
    picture_x = (width - picture_w) / 2;
    picture_y = (height - picture_h) / 2;

    *x = (window_x - picture_x) * MOUSE_WIDTH / (picture_w > 0 ? picture_w : 1);
    *y = (window_y - picture_y) * MOUSE_HEIGHT / (picture_h > 0 ? picture_h : 1);
    if (*x < 0) *x = 0;
    if (*y < 0) *y = 0;
    if (*x > MOUSE_WIDTH - 1) *x = MOUSE_WIDTH - 1;
    if (*y > MOUSE_HEIGHT - 1) *y = MOUSE_HEIGHT - 1;

    *buttons = 0;
    if (state & SDL_BUTTON(SDL_BUTTON_LEFT)) *buttons |= 1;
    if (state & SDL_BUTTON(SDL_BUTTON_RIGHT)) *buttons |= 2;
    if (state & SDL_BUTTON(SDL_BUTTON_MIDDLE)) *buttons |= 4;
}

int Game_DosInterrupt(uint8_t number, Game_DosRegs *regs)
{
    int x, y, buttons;

    if (number != 0x33) return 0;

    mouse_read(&x, &y, &buttons);

    switch (Game_DosAX(regs)) {
    case 0x0000:
        /* the driver is there: the buttons and the size of the screen */
        Game_DosSetAX(regs, 0);
        Game_DosSetBX(regs, 3);
        Game_DosSetCX(regs, MOUSE_WIDTH);
        Game_DosSetDX(regs, MOUSE_HEIGHT);
        break;
    case 0x0001:   /* the last press of a button */
    case 0x0002:   /* the last motion */
    case 0x0003:   /* the position and the buttons */
        Game_DosSetAX(regs, 0);
        Game_DosSetBX(regs, (uint16_t)buttons);
        Game_DosSetCX(regs, (uint16_t)(x * 2));
        Game_DosSetDX(regs, (uint16_t)y);
        break;
    case 0x0004:
        /* set the position (the X is doubled, as the driver counts it) */
        mouse_x = Game_DosCX(regs) / 2;
        mouse_y = Game_DosDX(regs);
        if (mouse_x < mouse_min_x) mouse_x = mouse_min_x;
        if (mouse_x > mouse_max_x) mouse_x = mouse_max_x;
        if (mouse_y < mouse_min_y) mouse_y = mouse_min_y;
        if (mouse_y > mouse_max_y) mouse_y = mouse_max_y;
        Game_DosSetAX(regs, 0);
        break;
    case 0x0005:
        /* the buttons pressed since the last call */
        Game_DosSetAX(regs, 0);
        Game_DosSetBX(regs, (uint16_t)buttons);
        Game_DosSetCX(regs, (uint16_t)(x * 2));
        Game_DosSetDX(regs, (uint16_t)y);
        break;
    case 0x0006:
        /* the shift keys */
        Game_DosSetAX(regs, 0);
        Game_DosSetBX(regs, 0);
        break;
    case 0x0007:
        /* the smallest and the largest X */
        mouse_min_x = Game_DosCX(regs);
        mouse_max_x = Game_DosDX(regs);
        Game_DosSetAX(regs, 0);
        break;
    case 0x0008:
        /* the smallest and the largest Y */
        mouse_min_y = Game_DosCX(regs);
        mouse_max_y = Game_DosDX(regs);
        Game_DosSetAX(regs, 0);
        break;
    case 0x000b:
        /* the motion counters since the last call */
        Game_DosSetAX(regs, 0);
        Game_DosSetCX(regs, 0);
        Game_DosSetDX(regs, 0);
        break;
    case 0x001c:
        /* the user's own handler of the mouse events; 0: the driver's own */
        mouse_handler = regs->ebx;
        Game_DosSetAX(regs, 0);
        break;
    default:
        Game_DosSetAX(regs, 0);
        break;
    }

    if (trace_mouse()) {
        fprintf(stderr, "MOUSE ax=%04x -> x=%d y=%d buttons=%d (the user handler %08x)\n",
                Game_DosAX(regs), x, y, buttons, mouse_handler);
    }
    Game_DosSetCarry(regs, 0);
    return 1;
}
