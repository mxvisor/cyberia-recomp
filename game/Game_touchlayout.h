/* The on-screen gamepad (see touchgamepad.c).  The keys are the game's, from
 * README.TXT of the disc: the arrows move between points, aim in the flying
 * scenes and walk the menus; Space fires, Enter chooses; F1-F3 are the scans
 * (infrared, magnetic resonance, biomass) and F10 stops one; Esc is the main
 * menu, L the load menu, P the pause.  The buttons sit in the black bars
 * beside the 4:3 picture, as small as the bars ask; a touch elsewhere is the
 * mouse.  With the mod Gyro_Mouse FIRE is the mouse's left button: the game
 * reads the mouse only while no arrow and no Space is held (loc_1D088), so
 * Space would stop the aim of the gyroscope. */

#include "Game_gyro.h"

#define GP_NBUTTONS 13

static GPButton gp_buttons[GP_NBUTTONS] = {
    {0,0,0,0, SDLK_UP,     1, NULL,    GP_LIGHT,  -1},
    {0,0,0,0, SDLK_DOWN,   2, NULL,    GP_LIGHT,  -1},
    {0,0,0,0, SDLK_LEFT,   3, NULL,    GP_LIGHT,  -1},
    {0,0,0,0, SDLK_RIGHT,  4, NULL,    GP_LIGHT,  -1},
    {0,0,0,0, SDLK_SPACE,  0, "FIRE",  GP_RED,    -1},
    {0,0,0,0, SDLK_RETURN, 0, "ENTER", GP_BLUE,   -1},
    {0,0,0,0, SDLK_F1,     0, "IR",    GP_ORANGE, -1},
    {0,0,0,0, SDLK_F2,     0, "MRI",   GP_ORANGE, -1},
    {0,0,0,0, SDLK_F3,     0, "BIO",   GP_ORANGE, -1},
    {0,0,0,0, SDLK_F10,    0, "STOP",  GP_GREY,   -1},
    {0,0,0,0, SDLK_ESCAPE, 0, "ESC",   GP_RED,    -1},
    {0,0,0,0, SDLK_l,      0, "LOAD",  GP_GREY,   -1},
    {0,0,0,0, SDLK_p,      0, "PAUSE", GP_GREY,   -1},
};

static void gp_place(int index, int x, int y, int w, int h)
{
    gp_buttons[index].x = x;
    gp_buttons[index].y = y;
    gp_buttons[index].w = w;
    gp_buttons[index].h = h;
}

static void gp_layout(int W, int H)
{
    int bar = (W > H * 4 / 3) ? (W - H * 4 / 3) / 2 : 0;
    int S = bar * 2 / 7;
    int gap, m, small, pw, aw, ah, x, y, i;

    /* the D-pad is 3.5 cells wide with its margins and must fit the bar; only a
     * screen without bars (4:3 or narrower) gets the buttons over the picture */
    if (bar == 0) S = H / 7;
    if (S < H / 10) S = H / 10;
    if (S > H / 5) S = H / 5;
    gap = S / 8;
    m = S / 4;
    small = S * 2 / 3;
    pw = 3 * S + 2 * gap;
    gp_buttons[4].key = Game_GyroMod() ? TOUCHGAMEPAD_MOUSE_LEFT : SDLK_SPACE;

    /* the D-pad at the bottom left */
    x = m;
    y = H - m - pw;
    gp_place(0, x + S + gap, y, S, S);
    gp_place(1, x + S + gap, y + 2 * (S + gap), S, S);
    gp_place(2, x, y + S + gap, S, S);
    gp_place(3, x + 2 * (S + gap), y + S + gap, S, S);

    /* fire and enter at the bottom right, the scans above them and the stop of a
     * scan above those: the block is as wide as the D-pad */
    x = W - m - pw;
    aw = (pw - gap) / 2;
    ah = S * 3 / 2;
    y = H - m - ah;
    gp_place(4, x, y, aw, ah);
    gp_place(5, x + aw + gap, y, aw, ah);
    y -= gap + small;
    for (i = 0; i < 3; i++)
    {
        gp_place(6 + i, x + i * (S + gap), y, S, small);
    }
    y -= gap + small;
    gp_place(9, x, y, S, small);

    /* the menus in the top corners, away from the thumbs */
    gp_place(10, m, m, S, small);
    gp_place(11, m + S + gap, m, S, small);
    gp_place(12, W - m - S, m, S, small);
}
