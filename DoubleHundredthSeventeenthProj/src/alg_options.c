/*
 * Elite - The New Kind.
 *
 * Reverse engineered from the BBC disk version of Elite.
 * Additional material by C.J.Pinder.
 *
 * The original Elite code is (C) I.Bell & D.Braben 1984.
 * This version re-engineered in C by C.J.Pinder 1999-2001.
 *
 * email: <christian@newkind.co.uk>
 *
 *
 */

/**
 * SDL/OpenGL Port by Mark Follett 2001-2002
 * email: <mef123@geocities.com>
 **/

/*
 * Options.c
 */

#include <stdlib.h>
#include <string.h>

#include "alg_elite.h"
#include "alg_config.h"
#include "alg_gfx.h"
#include "alg_options.h"
#include "alg_main.h"
#include "alg_docked.h"
#include "alg_file.h"

static int hilite_item;

#define NUM_OPTIONS 4
#define NUM_SETTINGS 6

#define OPTION_BAR_WIDTH	(400)
#define OPTION_BAR_HEIGHT	(15)

struct menu_option
{
	char *text;
	int docked_only;
};

static struct menu_option option_list[NUM_OPTIONS] =
{
	{"Save Commander",	1},
	{"Load Commander",	1},
	{"Game Settings",	0},
	{"Quit",			0}	
};

struct setting
{
	char *name;
	char *value[5];
};

static struct setting setting_list[NUM_SETTINGS] =
{
	{"Graphics:",		{"Solid", "Wireframe", "", "", ""}},
	{"Anti Alias:",		{"Off", "On", "", "", ""}},		
	{"Planet Style:",	{"Wireframe", "Green", "SNES", "Fractal", ""}},
	{"Planet Desc.:",	{"BBC", "MSX", "", "", ""}},
	{"Instant Dock:",	{"Off", "On", "", "", ""}},	
	{"Save Settings",	{"", "", "", "", ""}}
};


void quit_screen (void)
{
	gfx_display_centre_text (10, "GAME OPTIONS", 140, GFX_COL_GOLD);
	gfx_draw_line (0, 36, 511, 36);

	gfx_display_centre_text (175, "QUIT GAME (Y/N)?", 140, GFX_COL_GOLD);		
}





void display_setting_item (int item)
{
	int x,y;
	int v;

	if (item == (NUM_SETTINGS - 1))
	{
		y = ((NUM_SETTINGS + 1) / 2) * 30 + 96 + 32;
		gfx_display_centre_text (y, setting_list[item].name, 120, GFX_COL_WHITE);
		return;
	}
	
	switch (item)
	{
		case 0:
			v = wireframe;
			break;
		
		case 1:
			v = anti_alias_gfx;
			break;
		
		case 2:
			v = planet_render_style;
			break;
		
		case 3:
			v = hoopy_casinos;
			break;
		
		case 4:
			v = instant_dock;
			break;

		default:
			v = 0;
			break;
	}
	
	x = (item & 1) * 250 + 32; 
	y = (item / 2) * 30 + 96;
	
	gfx_display_colour_text (x, y, setting_list[item].name, GFX_COL_WHITE);
	gfx_display_colour_text (x + 120, y, setting_list[item].value[v], GFX_COL_WHITE);
}


void highlight_setting (int item)
{
	int x,y;
	int width;
	
	if (item == (NUM_SETTINGS - 1))
	{
		x = GFX_X_CENTRE - (OPTION_BAR_WIDTH / 2);
		y = ((NUM_SETTINGS + 1) / 2) * 30 + 96 + 32;
		width = OPTION_BAR_WIDTH;
	}
	else
	{
		x = (item & 1) * 250 + 32 + 120; 
		y = (item / 2) * 30 + 96;
		width = 100;
	}
	
	gfx_draw_rectangle (x, y, x + width, y + OPTION_BAR_HEIGHT, GFX_COL_DARK_RED);
	display_setting_item (item);		
}



void select_left_setting (void)
{
	if ((hilite_item & 1) != 0)
        hilite_item--;
}

void select_right_setting (void)
{
	if (((hilite_item & 1) == 0) && (hilite_item < (NUM_SETTINGS - 1)))
        hilite_item++;
}


void select_up_setting (void)
{
	if (hilite_item == (NUM_SETTINGS - 1))
	{
        hilite_item = NUM_SETTINGS - 2;
		return;
	}

	if (hilite_item > 1)
        hilite_item = hilite_item - 2;
}


void select_down_setting (void)
{
	if (hilite_item == (NUM_SETTINGS - 2))
	{
        hilite_item = NUM_SETTINGS - 1;
		return;
	}
	
	if (hilite_item < (NUM_SETTINGS - 2))
        hilite_item = hilite_item + 2;
}

void toggle_setting (void)
{
	if (hilite_item == (NUM_SETTINGS - 1))
	{
		write_config_file();
        switch_to_screen(SCR_OPTIONS);
		return;
	}

	switch (hilite_item)
	{
		case 0:
			wireframe ^= 1;
			break;
		
		case 1:
			anti_alias_gfx ^= 1;
			break;
		
		case 2:
			planet_render_style = (planet_render_style + 1) % 4;
			break;
		
		case 3:
			hoopy_casinos ^= 1;
			break;

		case 4:
			instant_dock ^= 1;
			break;
	}

}


void game_settings_screen (void)
{
	int i;

	gfx_display_centre_text (10, "GAME SETTINGS", 140, GFX_COL_GOLD);
	gfx_draw_line (0, 36, 511, 36);

	for (i = 0; i < NUM_SETTINGS; i++)
	{
		display_setting_item (i);
	}

    highlight_setting (hilite_item);
}


void display_option_item (int i)
{
	int y;
	int col;
	
	y = (384 - (30 * NUM_OPTIONS)) / 2;
	y += i * 30;
	col = ((!docked) && option_list[i].docked_only) ? GFX_COL_GREY_1 : GFX_COL_WHITE;

	gfx_display_centre_text (y, option_list[i].text, 120, col);
}


void highlight_option (int i)
{
	int y;
    int x;
    x = GFX_X_CENTRE - (OPTION_BAR_WIDTH / 2);
	y = (384 - (30 * NUM_OPTIONS)) / 2;
	y += i * 30;

	gfx_draw_rectangle (x, y, x + OPTION_BAR_WIDTH, y + OPTION_BAR_HEIGHT,
                        GFX_COL_DARK_RED);
	display_option_item (i);
}

void select_previous_option (void)
{
	if (hilite_item > 0)
		hilite_item--;
}

void select_next_option (void)
{
	if (hilite_item < (NUM_OPTIONS - 1))
		hilite_item++;
}


void do_option (void)
{
	if ((!docked) && option_list[hilite_item].docked_only)
		return;

	switch (hilite_item)
	{
		case 0:
			switch_to_screen(SCR_SAVE_CMDR);
			break;
			
		case 1:
			switch_to_screen(SCR_LOAD_CMDR);
			break;
		
		case 2:
			switch_to_screen(SCR_SETTINGS);
			break;

		case 3:
			switch_to_screen(SCR_QUIT);
			break;
	}
}


void display_options (void)
{
	int i;

	gfx_display_centre_text (10, "GAME OPTIONS", 140, GFX_COL_GOLD);
	gfx_draw_line (0, 36, 511, 36);
	gfx_display_centre_text (300, "Version: Release 1.0.0", 120, GFX_COL_WHITE);
	gfx_display_centre_text (320, "Written by Christian Pinder 1999-2001", 120, GFX_COL_WHITE);
	gfx_display_centre_text (340, "SDL/OpenGL Port by Mark Follett", 120, GFX_COL_WHITE);
	gfx_display_centre_text (360, "Based on original code by Ian Bell & David Braben", 120, GFX_COL_WHITE);
	
	for (i = 0; i < NUM_OPTIONS; i++)
		display_option_item (i);

    highlight_option(hilite_item);
}


void initialize_options_screen (void)
{
    hilite_item = 0;
}


void initialize_settings_screen (void)
{
    hilite_item = 0;
}
