/* The port's mods (off by default), each a key of cyberia.cfg and an item of
 * the port's panel. */

#include <stdint.h>
#include <stdio.h>
#include <strings.h>

#include "Game_hooks.h"
#include "Game_osd.h"

/* [40278] (global_aliases.sci): 1 turns the crosshair upside down, Y = 199 - Y
 * of the mouse, the arrows and the joystick (loc_1D088); the game starts with
 * 1, and F flips it. */
extern "C" uint8_t cyb_crosshair_inverted;

/* Uninvert_Crosshair: the crosshair goes where the mouse (a touch) goes */
static int uninvert_mod;

static void uninvert_apply(void)
{
    cyb_crosshair_inverted = uninvert_mod ? 0 : 1;
}

int CYB_ConfigKey(const char *key, const char *value)
{
    if (strcasecmp(key, "Uninvert_Crosshair") != 0) return 0;
    if (strcasecmp(value, "on") == 0) uninvert_mod = 1;
    else if (strcasecmp(value, "off") == 0) uninvert_mod = 0;
    else fprintf(stderr, GAME_NAME ": unknown Uninvert_Crosshair: %s\n", value);
    uninvert_apply();
    return 1;
}

static void uninvert_press(void)
{
    uninvert_mod = !uninvert_mod;
    uninvert_apply();
}

static const char *uninvert_state(void)
{
    return uninvert_mod ? "on" : "off";
}

static const Game_OsdItem uninvert_item = { "Uninverted crosshair", NULL, uninvert_press, uninvert_state, "Uninvert_Crosshair", NULL, NULL, NULL, NULL };

void CYB_OsdItems(void)
{
    Game_OsdAddItem(&uninvert_item);
}
