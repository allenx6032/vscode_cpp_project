#include <darnit_api.h>
#include <string.h>

#include "teleport.h"
#include "world.h"
#include "save_loader.h"
#include "character.h"
#include "camera.h"
#include "game_item.h"


void world_dungeon_load(int dungeon) {
	/* TODO: Fix */
	dungeon_unload();
	my_ws.active_world = dungeon;
	dungeon_load(dungeon);
}


void world_init() {
	my_ws.active_world = -1;
	my_ws.state = WORLD_STATE_MAINMENU;
	my_ws.new_state = WORLD_STATE_MAINMENU;
	my_ws.dm = NULL;
	my_ws.textbox = NULL;

	my_ws.font = d_font_load("res/mainfont.ttf", MAIN_FONT_SIZE, 512, 512);
	my_ws.te.font = d_font_load("res/mainfont.ttf", TEXTEFFECT_FONT_SIZE, 256, 256);
	ambiance_init();
}


void world_reset() {
	my_ws.active_world = -1;
	dungeon_unload(my_ws.dm);
	my_ws.camera.follow_char = -1;
	character_destroy();
	textbox_destroy();
	texteffect_done();
	item_destroy(my_ws.item);

	return;
}


void world_save() {
	char fname[256];

	sprintf(fname, "world_%i.save", my_ws.savedata.world);
	savedata_save(fname);

	return;
}



void world_load(int world_num) {
	char fname[256];

	my_ws.savedata.is = my_ws.savedata.bs = 0;

	my_ws.loading_from_save = 1;
	sprintf(fname, "world_%i.save", world_num);
	d_fs_mount(fname);
	character_init();
	camera_init();
	my_ws.camera.sys = d_render_tilesheet_load("res/ui.png", my_ws.camera.tile_w, my_ws.camera.tile_h,
		DARNIT_PFORMAT_RGB5A1);
	dungeon_init();
	texteffect_init();

	/* Just testing textbox */
	textbox_init(800, 96, 0, 480-96, 10, 10, 10, 10);

	save_load_deps();
	teleport_load();
	savedata_load(fname);

	my_ws.savedata.world = world_num;
	/* FIXME: Temporary hack to test code */
	my_ws.camera.follow_char = 0;
	my_ws.camera.player = 0;
	my_ws.active_world = 0;
	savedata_save(fname);

	item_init("res/item.list");
	my_ws.inv = inventory_new(1);
	my_ws.inv->entry[0].type = 0;
	my_ws.inv->entry[0].amount = 1;


	return;
}


void world_loop() {
	int i;
	struct teleport_to t;	/* T för Taliban att Teleportera! */

	if (my_ws.new_state != my_ws.state) {
		if (!d_render_fade_status())
			d_render_fade_in(WORLD_FADE_TIME, 0, 0, 0);
		else if (d_render_fade_status() == 2) {
			switch (my_ws.state) {
				case WORLD_STATE_MAINMENU:
					break;
				case WORLD_STATE_MAPSTATE:
					dungeon_unload();
				case WORLD_STATE_CHANGEMAP:
					/* TODO: Insert music playback */
					dungeon_unload();
					dungeon_load(4);
					t = my_ws.char_data->teleport.to;
					t.x *= my_ws.camera.tile_w;
					t.y *= my_ws.camera.tile_h;
					if (t.slot > 0)
						character_spawn_entry(t.slot, t.ai, t.x, t.y, t.l, 4, -1);
					t.slot = -1;
					my_ws.new_state = WORLD_STATE_MAPSTATE;
					break;
				default:
					break;
			}

			switch (my_ws.new_state) {
				case WORLD_STATE_MAINMENU:
					break;
				case WORLD_STATE_MAPSTATE:
					dungeon_load(4);
					/* TODO: Replace with proper code */
					character_spawn_entry(0, "player_ai", 400, 400, 0, 0, -1);
					my_ws.loading_from_save = 0;
					break;
				case WORLD_STATE_CHANGEMAP:
				default:
					break;
			}

			my_ws.state = my_ws.new_state;
			d_render_fade_out(WORLD_FADE_TIME);
		}
	}
			


	switch (my_ws.state) {
		case WORLD_STATE_MAINMENU:
			/* TODO: Implement */
			break;
		case WORLD_STATE_MAPSTATE:
			character_loop();
			camera_loop();
			textbox_loop();

			d_render_begin();
			for (i = 0; i < my_ws.dm->grid[4].layers; i++) {
				d_tilemap_camera_move(my_ws.dm->grid[4].layer[i], my_ws.camera.x, my_ws.camera.y);
				d_tilemap_draw(my_ws.dm->grid[4].layer[i]);
				/* NOTE: This depends on the collision buffer not changing */
				d_render_offset(my_ws.camera.x, my_ws.camera.y);
				d_render_blend_enable();
				character_render_layer(character_find_visible(), i);
				d_render_blend_disable();
				d_render_offset(0, 0);
			}
	
			d_render_blend_enable();
			d_render_offset(my_ws.camera.x, my_ws.camera.y);
			texteffect_loop();
			d_render_offset(0, 0);
			textbox_draw();
			d_render_blend_disable();
			if (d_keys_get().rmb)
				item_use(my_ws.inv, 0, 0, 0);

			/* TODO: Render a UI of some sort */
			d_render_end();
		default:
			break;
	}

	return;
}


int world_calc_tile(int x, int y, int l) {
	x /= my_ws.camera.tile_w;
	y /= my_ws.camera.tile_h;
	if (x < 0 || y < 0) {
		return -1;
	}

	switch (my_ws.state) {
		case WORLD_STATE_MAPSTATE:
			if (x >= my_ws.dm->grid[4].layer[l]->w)
				return -1;
			if (y >= my_ws.dm->grid[4].layer[l]->h)
				return -1;
			return x + y * my_ws.dm->grid[4].layer[l]->w;
			break;
		default:
			return 0;
	}

	return 0;
}


unsigned int world_get_tile_i(int i, int l) {
	if (i < 0)
		return ~0;
	switch (my_ws.state) {
		case WORLD_STATE_MAPSTATE:
			return my_ws.dm->grid[4].layer[l]->data[i];
			break;
		default:
			return 0;
			break;
	}

	return 0;
}


unsigned int world_get_tile(int x, int y, int l) {
	return world_get_tile_i(world_calc_tile(x, y, l), l);
}


