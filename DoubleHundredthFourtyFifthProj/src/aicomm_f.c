#include "character.h"
#include "aicomm_f.h"
#include "aicomm.h"
#include "savefile.h"
#include "textbox.h"
#include "world.h"
#include <string.h>


int character_get_character_looked_at(int src);
struct aicomm_struct character_message_next(struct aicomm_struct ac);
void character_update_sprite(int entry);
void character_despawn(int entry);


struct aicomm_struct aicomm_f_diru(struct aicomm_struct ac) {
	if (ac.from < 0 || ac.from >= my_ws.char_data->max_entries ||
	    !my_ws.char_data->entry[ac.from]) {
		ac.msg = AICOMM_MSG_NOAI;
		ac.self = ac.from;
		ac.from = -1;
		return ac;
	}

	character_update_sprite(ac.from);
	return character_message_next(ac);
}


struct aicomm_struct aicomm_f_nspr(struct aicomm_struct ac) {
	if (ac.from < 0 || ac.from >= my_ws.char_data->max_entries ||
	    !my_ws.char_data->entry[ac.from]) {
		ac.msg = AICOMM_MSG_NOAI;
		ac.self = ac.from;
		ac.from = -1;
		return ac;
	}

	d_sprite_free(my_ws.char_data->entry[ac.from]->sprite);
	character_unload_graphics(my_ws.char_data->entry[ac.from]->slot);
	my_ws.char_data->entry[ac.from]->slot = ac.arg[0];
	character_load_graphics(my_ws.char_data->entry[ac.from]->slot);
	character_enable_graphics(ac.from);
	character_update_sprite(ac.from);

	return character_message_next(ac);
}


struct aicomm_struct aicomm_f_tpme(struct aicomm_struct ac) {
	struct savefile_teleport_entry t;
	
	if (ac.arg[0] >= (signed) my_ws.char_data->teleport.entries) {
		ac.self = ac.from;
		ac.from = -1;
		ac.msg = AICOMM_MSG_INVM;
		return ac;
	}
	
	/* TODO: Add teleport ID offset */
	t = my_ws.char_data->teleport.entry[ac.arg[0]];

	if (t.map == my_ws.active_world) {
		ac.ce[ac.from]->x = (t.x << 8) * my_ws.camera.tile_w;
		ac.ce[ac.from]->y = (t.y << 8) * my_ws.camera.tile_h;
		ac.ce[ac.from]->l = t.l;
		return character_message_next(ac);
	}

	my_ws.new_state = WORLD_STATE_CHANGEMAP;
	
	my_ws.char_data->teleport.to.slot = ac.ce[ac.from]->slot;
	strcpy(my_ws.char_data->teleport.to.ai, ac.ce[ac.from]->ai);
	my_ws.char_data->teleport.to.x = t.x;
	my_ws.char_data->teleport.to.y = t.y;
	my_ws.char_data->teleport.to.l = t.l;
	ac.ce[ac.from]->map = t.map;
	my_ws.char_data->teleport.to.dungeon = t.map;

	return character_message_next(ac);
}


struct aicomm_struct aicomm_f_folm(struct aicomm_struct ac) {
	my_ws.camera.follow_char = ac.from;
	ac.self = ac.from;
	ac.from = -1;
	ac.msg = AICOMM_MSG_NEXT;

	return character_message_next(ac);
}


struct aicomm_struct aicomm_f_setp(struct aicomm_struct ac) {
	my_ws.camera.player = ac.self;

	return character_message_next(ac);
}


struct aicomm_struct aicomm_f_getp(struct aicomm_struct ac) {
	ac.self = ac.from;
	ac.from = my_ws.camera.player;
	
	return ac;
}


struct aicomm_struct aicomm_f_kill(struct aicomm_struct ac) {
	character_despawn(ac.self);
	return character_message_next(ac);
}


struct aicomm_struct aicomm_f_spwn(struct aicomm_struct ac) {
	int x, y;

	x = ac.arg[1] * my_ws.camera.tile_w;
	y = ac.arg[2] * my_ws.camera.tile_h;
	character_spawn_entry(ac.arg[0], ac.argp, x, y, ac.arg[3], my_ws.dm->grid[4].id, -1);

	return character_message_next(ac);
}


struct aicomm_struct aicomm_f_getf(struct aicomm_struct ac) {
	ac.self = ac.from;
	ac.from = character_get_character_looked_at(ac.self);

	return ac;
}


struct aicomm_struct aicomm_f_camn(struct aicomm_struct ac) {
	my_ws.camera.jump = 1;

	return character_message_next(ac);
}


struct aicomm_struct aicomm_f_tbox(struct aicomm_struct ac) {
	struct textbox_properties *tp;

	tp = ac.argp;
	if (tp)
		textbox_add_message(tp->message, tp->question, ac.arg[0], ac.from);
	return character_message_next(ac);
}


struct aicomm_struct aicomm_f_invm(struct aicomm_struct ac) {
	ac.self = ac.from;
	ac.from = -1;
	ac.msg = AICOMM_MSG_INVM;

	return ac;
}


struct aicomm_struct aicomm_f_txte(struct aicomm_struct ac) {
	ac.arg[0] = texteffect_add(ac.argp, ac.arg[0], ac.arg[1], ac.arg[2], ac.arg[3], ac.arg[4], ac.arg[5], ac.arg[6]);
	
	ac.self = ac.from;
	ac.from = -1;
	ac.msg = AICOMM_MSG_TXTE;

	return ac;
}


struct aicomm_struct aicomm_f_prel(struct aicomm_struct ac) {
	struct character_entry *ce;
	void *resource;

	ce = my_ws.char_data->entry[ac.from];
	ac.self = ac.from;
	ac.from = -1;
	if (!(resource = character_preload_load(ac.argp, ac.arg[0], ac.arg[1], ac.arg[2]))) {
		ac.arg[0] = 0;
		return ac;
	}

	ce->char_preload = realloc(ce->char_preload, sizeof(*ce->char_preload) * (ce->char_preloads + 1));
	ce->char_preload[ce->char_preloads].name = strdup(ac.argp);
	ce->char_preload[ce->char_preloads].resource = resource;
	ce->char_preload[ce->char_preloads].cr = ac.arg[0];

	ce->char_preloads++;
	ac.arg[0] = 1;
	return ac;
}


struct aicomm_struct aicomm_f_unlo(struct aicomm_struct ac) {
	ac.self = ac.from;
	ac.from = -1;
	character_preload_free(my_ws.char_data->entry[ac.self], ac.argp);
	return ac;
}


struct aicomm_struct aicomm_f_dummy(struct aicomm_struct ac) {
	return ac;
}

