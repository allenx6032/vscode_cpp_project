#ifndef __WORLD_H__
#define	__WORLD_H__

#include "save_loader.h"
#include "camera.h"
#include "savedata.h"
#include "textbox.h"
#include "texteffects.h"
#include "game_item.h"
#include "ambiance.h"

#define	WORLD_FADE_TIME			500
#define	MAIN_FONT_SIZE			24

enum world_state_e {
	WORLD_STATE_MAINMENU,
	WORLD_STATE_MAPSTATE,
	WORLD_STATE_CHANGEMAP,
};

// typedef struct world_state {
// 	enum world_state_e		state;
// 	enum world_state_e		new_state;
// 	int				active_world;
// 	DARNIT_FONT			*font;
// 	struct dungeon_map		*dm;
// 	struct character_data		*char_data;
// 	struct camera			camera;
// 	struct savedata			savedata;
// 	struct textbox			*textbox;
// 	struct texteffect		te;
// 	struct item			item;
// 	struct ambiance			ambiance;
// 	int				loading_from_save;


// 	/* Just testing */
// 	struct inventory		*inv;
// } WS,*WS_ptr;

// WS my_ws;

struct world_state {
	enum world_state_e		state;
	enum world_state_e		new_state;
	int				active_world;
	DARNIT_FONT			*font;
	struct dungeon_map		*dm;
	struct character_data		*char_data;
	struct camera			camera;
	struct savedata			savedata;
	struct textbox			*textbox;
	struct texteffect		te;
	struct item			item;
	struct ambiance			ambiance;
	int				loading_from_save;


	/* Just testing */
	struct inventory		*inv;
};

static struct world_state my_ws = {};

void world_load(int world_num);
void world_save();
void world_init();
void world_loop();
int world_calc_tile(int x, int y, int l);
unsigned int world_get_tile(int x, int y, int l);
unsigned int world_get_tile_i(int i, int l);

#endif
