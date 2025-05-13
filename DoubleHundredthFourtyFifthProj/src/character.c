#include <darnit_api.h>
#include <string.h>
#include "character.h"
#include "world.h"
#include "aicomm.h"
#include "aicomm_f.h"
#include "aicomm_handlers.h"


void character_expand_entries();
void character_update_sprite(int entry);


int character_get_character_looked_at(int src) {
	int x, y, w, h, xt, yt, n, i;
	unsigned int t[9];

	if (src < 0 || src >= my_ws.char_data->max_entries)
		return -1;
	if (!my_ws.char_data->entry[src])
		return -1;
	d_sprite_hitbox(my_ws.char_data->entry[src]->sprite, &x, &y, &w, &h);
	x += (my_ws.char_data->entry[src]->x >> 8);
	y += (my_ws.char_data->entry[src]->y >> 8);

	switch (my_ws.char_data->entry[src]->dir) {
		case 0:	/* West */
			xt = x - (w >> 1);
			yt = y + (h >> 1);
			break;
		case 1:	/* North */
			xt = x + (w >> 1);
			yt = y - (h >> 1);
			break;
		case 2:	/* East */
			xt = x + w;
			yt = y + (h >> 1);
			break;
		case 3:	/* South */
			xt = x + (w >> 1);
			yt = y + h;
			break;
		case 4: /* North-west */
			xt = x - (w >> 1);
			yt = y - (h >> 1);
			break;
		case 5:	/* North-east */
			xt = x + w;
			yt = y - (h >> 1);
			break;
		case 6:	/* South-east */
			xt = x + w;
			yt = y + h;
			break;
		case 7:	/* South-west */
			xt = x - (w >> 1);
			yt = y + h;
			break;
		default:
			return -1;
			break;
	}

	n = d_bbox_test(my_ws.char_data->bbox, xt, yt, (w >> 1), (h >> 1), t, 9);
	if (!n)
		return -1;
	for (i = 0; i < 9; i++)
		if (my_ws.char_data->entry[t[i]]->map == my_ws.dm->grid[4].id)
			return t[i];
	return -1;
}
			


int character_load_ai_lib(const char *fname) {
	int i;

	i = my_ws.char_data->ai_libs++;
	my_ws.char_data->ai_lib = realloc(my_ws.char_data->ai_lib, 
		sizeof(*my_ws.char_data->ai_lib) * my_ws.char_data->ai_libs);
	if (!(my_ws.char_data->ai_lib[i].lib = d_dynlib_open(fname))) {
		my_ws.char_data->ai_libs--;
		return 0;
	}

	my_ws.char_data->ai_lib[i].ainame = malloc(strlen(fname) + 1);
	strcpy(my_ws.char_data->ai_lib[i].ainame, fname);

	return 1;
}


void *character_find_ai_func(const char *name) {
	int i;
	void *func = NULL;

	for (i = 0; i < my_ws.char_data->ai_libs && !func; i++)
		func = d_dynlib_get(my_ws.char_data->ai_lib[i].lib, name);
	return func;
}


void character_init() {
	unsigned int chars;
	int i;

	chars = character_gfx_data_characters();
	my_ws.char_data = malloc(sizeof(*my_ws.char_data));
	my_ws.char_data->characters = chars;
	my_ws.char_data->gfx = calloc(sizeof(*my_ws.char_data->gfx) * chars, 1);
	my_ws.char_data->max_entries = 8;
	my_ws.char_data->entries = 0;
	my_ws.char_data->entry = malloc(sizeof(*my_ws.char_data->entry) * my_ws.char_data->max_entries);
	my_ws.char_data->collision = malloc(sizeof(unsigned int) * my_ws.char_data->max_entries);
	my_ws.char_data->bbox = d_bbox_new(my_ws.char_data->max_entries);
	my_ws.char_data->ai_lib = NULL;
	my_ws.char_data->ai_libs = 0;
	d_bbox_sortmode(my_ws.char_data->bbox, DARNIT_BBOX_SORT_Y);

	for (i = 0; i < my_ws.char_data->max_entries; i++)
		my_ws.char_data->entry[i] = NULL;
	character_expand_entries();

	return;
}


void character_destroy() {
	unsigned int i;

	for (i = 0; (signed) i < my_ws.char_data->max_entries; i++)
		character_despawn(i);
	free(my_ws.char_data->entry);
	for (i = 0; i < my_ws.char_data->characters; i++)
		character_unload_graphics(i);
	for (i = 0; (signed) i < my_ws.char_data->ai_libs; i++) {
		my_ws.char_data->ai_lib[i].lib = d_dynlib_close(my_ws.char_data->ai_lib[i].lib);
		free(my_ws.char_data->ai_lib[i].ainame);
	}

	free(my_ws.char_data->ai_lib);
	free(my_ws.char_data->gfx);
	free(my_ws.char_data->collision);
	d_bbox_free(my_ws.char_data->bbox);
	free(my_ws.char_data);
	my_ws.char_data = NULL;

	return;
}


struct aicomm_struct character_message_next(struct aicomm_struct ac) {
	ac.self = ac.from;
	ac.from = -1;
	ac.msg = AICOMM_MSG_NEXT;

	return ac;
}


void character_message_loop(struct aicomm_struct ac) {
	for (;;) {
		ac.ce = my_ws.char_data->entry;
		if (ac.self < 0)
			return;
		else if (!my_ws.char_data->entry[ac.self] || !my_ws.char_data->entry[ac.self]->loop) {
			ac.msg = AICOMM_MSG_NOAI;
			if (ac.from < 0 || ac.from >= my_ws.char_data->max_entries)
				return;
			if (!my_ws.char_data->entry[ac.from])
				return;
			if (!my_ws.char_data->entry[ac.from]->loop)
				return;
			ac.self = ac.from;
			ac.from = -1;
			ac = my_ws.char_data->entry[ac.from]->loop(ac);
		} else
			ac = my_ws.char_data->entry[ac.self]->loop(ac);

		switch (ac.msg) {
			case AICOMM_MSG_DONE:
				return;
			case AICOMM_MSG_INIT:
			case AICOMM_MSG_NOAI:
			case AICOMM_MSG_DESTROY:
			case AICOMM_MSG_SILE:
			case AICOMM_MSG_BOXR:
			case AICOMM_MSG_LOOP:
				/* Invalid return messages */
				fprintf(stderr, "WARNING: char %i returned invalid message %i\n",
					ac.from, ac.msg);
				return;
			default:
				if (ac.msg >= AICOMM_MSG_END)
					ac = aicomm_f_invm(ac);
				else if (!aihandle[ac.msg])
					ac = aicomm_f_invm(ac);
				else
					ac = aihandle[ac.msg](ac);
				break;
		}

	}

	return;
}


void character_tell_all(struct aicomm_struct ac) {
	int i;

	for (i = 0; i < my_ws.char_data->max_entries; i++)
		ac.self = i, character_message_loop(ac);
	return;
}


void character_set_hitbox(int entry) {
	int x, y, w, h;
	struct character_entry *ce;

	if (!my_ws.char_data->entry[entry])
		return;

	/* If the behaviour of darnit bbox ever changes, this will break */
	ce = my_ws.char_data->entry[entry];
	d_sprite_hitbox(ce->sprite, &x, &y, &w, &h);
	x += (ce->x >> 8);
	y += (ce->y >> 8);
	d_bbox_move(my_ws.char_data->bbox, entry, x, y);
	d_bbox_resize(my_ws.char_data->bbox, entry, (unsigned) w, (unsigned) h);

	return;
}


void character_expand_entries() {
	int i, nz;

	nz = (my_ws.char_data->max_entries << 1);
	my_ws.char_data->entry = realloc(my_ws.char_data->entry, nz * sizeof(*my_ws.char_data->entry));
	my_ws.char_data->collision = realloc(my_ws.char_data->collision, nz * sizeof(unsigned int));
	for (i = 0; i < my_ws.char_data->max_entries; i++)
		my_ws.char_data->entry[i + my_ws.char_data->max_entries] = NULL;
	
	my_ws.char_data->max_entries <<= 1;
	d_bbox_free(my_ws.char_data->bbox);
	my_ws.char_data->bbox = d_bbox_new(my_ws.char_data->max_entries);
	d_bbox_sortmode(my_ws.char_data->bbox, DARNIT_BBOX_SORT_Y);

	for (i = 0; i < my_ws.char_data->max_entries; i++)
		d_bbox_add(my_ws.char_data->bbox, ~0, ~0, 0, 0);
	
	for (i = 0; i < (my_ws.char_data->max_entries >> 1); i++)
		character_set_hitbox(i);
	
	return;
}


int character_load_graphics(unsigned int slot) {
	if (slot >= my_ws.char_data->characters)
		return 0;
	if (!my_ws.char_data->gfx[slot])
		my_ws.char_data->gfx[slot] = character_gfx_data_load(slot);

	my_ws.char_data->gfx[slot]->link++;
	return 1;
}


int character_unload_graphics(unsigned int slot) {
	if (slot >= my_ws.char_data->characters)
		return 0;
	if (!my_ws.char_data->gfx[slot])
		return 0;
	my_ws.char_data->gfx[slot]->link--;
	if (!my_ws.char_data->gfx[slot]->link)
		my_ws.char_data->gfx[slot] = character_gfx_data_unload(my_ws.char_data->gfx[slot]);
	return 1;
}


int character_test_map(int entry, int dx, int dy) {
	int x, y, w, h, x2, y2, t1, t2, t3, t4, dir, d1, d2;
	struct character_entry *ce;
	struct aicomm_struct ac;
	
	if (d_keys_get().l)
		return 0;

	ce = my_ws.char_data->entry[entry];
	d_sprite_hitbox(ce->sprite, NULL, NULL, &w, &h);
	x2 = ((ce->x + dx) >> 8);
	y2 = ((ce->y + dy) >> 8);
	x = (ce->x >> 8);
	y = (ce->y >> 8);
	if (x < 0 || x2 < 0 || y < 0 || y2 < 0)
		return 1;
	
	if (!dx) {
		x2 += (w - 1);
		y += (dy > 0) ? h - 1 : 0;
		y2 += (dy > 0) ? h - 1 : 0;
		t1 = world_calc_tile(x, y, ce->l);
		t2 = world_calc_tile(x, y2, ce->l);
		t3 = world_calc_tile(x2, y, ce->l);
		t4 = world_calc_tile(x2, y2, ce->l);
		dir = (((dy < 0) ? 0x8 : 0x2) << 16);
	} else if (!dy) {
		y2 += (h - 1);
		x += (dx > 0) ? w - 1 : 0;
		x2 += (dx > 0) ? w - 1 : 0;
		t1 = world_calc_tile(x, y, ce->l);
		t2 = world_calc_tile(x2, y, ce->l);
		t3 = world_calc_tile(x, y2, ce->l);
		t4 = world_calc_tile(x2, y2, ce->l);
		dir = (((dx < 0) ? 0x4 : 0x1) << 16);
	}
		
	if (t1 < 0 || t2 < 0 || t3 < 0 || t4 < 0)
		return 1;
	if (t1 == t2 && t3 == t4)
		return 0;

	d1 = world_get_tile_i(t2, ce->l);
	d2 = world_get_tile_i(t4, ce->l);

	ac.from = -1;
	ac.msg = AICOMM_MSG_MAPE;
	ac.self = entry;
	if (d1 & MAP_FLAG_EVENT) {
		ac.arg[0] = t2;
		ac.arg[1] = d1;
		character_message_loop(ac);
	}

	if (d2 & MAP_FLAG_EVENT) {
		ac.arg[0] = t4;
		ac.arg[1] = d2;
		character_message_loop(ac);
	}
	
	return ((d1 & dir) || (d2 & dir));
}


int character_test_collision(int entry, int dx, int dy) {
	struct aicomm_struct ac;
	struct character_entry *ce;
	int x, y, w, h, e, i, s, n;

	if (d_keys_get().l)
		return 0;

	ce = my_ws.char_data->entry[entry];
	d_sprite_hitbox(ce->sprite, &x, &y, &w, &h);
	x += ((ce->x + dx) >> 8);
	y += ((ce->y + dy) >> 8);
	n = d_bbox_test(my_ws.char_data->bbox, x, y, w, h, (unsigned *) my_ws.char_data->collision, 
		my_ws.char_data->max_entries);
	
	ac.msg = AICOMM_MSG_COLL;
	ac.from = -1;
	
	for (i = s = 0; i < n; i++) {
		e = my_ws.char_data->collision[i];
		if (my_ws.char_data->entry[e]->l != my_ws.char_data->entry[ce->self]->l)
			continue;
		if (e == ce->self)
			continue;
		if (my_ws.char_data->entry[e]->map != my_ws.dm->grid[4].id)
			continue;
		ac.arg[0] = ce->self;
		ac.self = e;
		character_message_loop(ac);
		ac.arg[0] = e;
		ac.self = ce->self;
		character_message_loop(ac);
		if (my_ws.char_data->entry[entry]->special_action.solid
		    && my_ws.char_data->entry[e]->special_action.solid)
			s = 1;
		
	}

	return s;
}


void character_update_sprite(int entry) {
	int x, y, w, h;
	struct character_entry *ce;

	ce = my_ws.char_data->entry[entry];

	d_sprite_hitbox(ce->sprite, &x, &y, &w, &h);
	x *= -1;
	y *= -1;
	x += (ce->x >> 8);
	y += (ce->y >> 8);

	d_sprite_direction_set(ce->sprite, ce->dir);
	(ce->special_action.animate ? d_sprite_animate_start : d_sprite_animate_stop)(ce->sprite);
	d_sprite_move(ce->sprite, x, y);
	d_bbox_move(my_ws.char_data->bbox, ce->self, x, y);
	d_bbox_resize(my_ws.char_data->bbox, ce->self, w, h);

	return;
}


void character_handle_movement(int entry) {
	int dx, dy;
	struct character_entry *e;

	e = my_ws.char_data->entry[entry];
	dx = e->dx;
	dy = e->dy;

	if (!dx && !dy)
		return;

	dx *= d_last_frame_time();
	dy *= d_last_frame_time();
	dx /= 1000;
	dy /= 1000;

	while (dx) {
		if (!character_test_collision(entry, dx, 0) && !character_test_map(entry, dx, 0)) {
			my_ws.char_data->entry[entry]->x += dx;
			break;
		}
		if (!(dx / 256))
			break;
		dx += (dx < 0) ? 256 : -256;
	}

	while (dy) {
		if (!character_test_collision(entry, 0, dy) && !character_test_map(entry, 0, dy)) {
			my_ws.char_data->entry[entry]->y += dy;
			break;
		}
		if (!(dy / 256))
			break;
		dy += (dy < 0) ? 256 : -256;
	}

	character_update_sprite(entry);

	e->dx = 0, e->dy = 0;
	
	return;
}


void character_enable_graphics(int entry) {
	int i, j, k, h, slot;
	struct char_gfx *cg;
	struct {
		int		tile;
		int		time;
	} *sprite;

	slot = my_ws.char_data->entry[entry]->slot;
	cg = my_ws.char_data->gfx[slot];
	sprite = (void *) my_ws.char_data->gfx[my_ws.char_data->entry[entry]->slot]->sprite_data;
	my_ws.char_data->entry[entry]->sprite = d_sprite_new(my_ws.char_data->gfx[my_ws.char_data->entry[entry]->slot]->sprite_ts);
	
	for (i = j = k = 0; sprite[i].tile != -1 || sprite[i].time != -1; i++) {
		h = j * 4;
		if (sprite[i].tile >= 0) {
			d_sprite_hitbox_set(my_ws.char_data->entry[entry]->sprite, j, k, cg->sprite_hitbox[h], 
			    cg->sprite_hitbox[h+1], cg->sprite_hitbox[h+2],cg->sprite_hitbox[h+3]);
			d_sprite_frame_entry_set(my_ws.char_data->entry[entry]->sprite, j, k++, sprite[i].tile, sprite[i].time);
		} else
			j++, k = 0;
	}
	
	d_sprite_activate(my_ws.char_data->entry[entry]->sprite, 0);

	return;
}


int character_spawn_entry(unsigned int slot, const char *ai, int x, int y, int l, int map, int save) {
	int i, j, k, h;
	struct character_entry *ce;
	struct char_gfx *cg;
	struct aicomm_struct ac;
	struct {
		int		tile;
		int		time;
	} *sprite;

	if (my_ws.char_data->characters <= slot)
		return -1;

	character_load_graphics(slot);
	sprite = (void *) my_ws.char_data->gfx[slot]->sprite_data;
	ce = malloc(sizeof(*ce));
	ce->sprite = d_sprite_new(my_ws.char_data->gfx[slot]->sprite_ts);
	cg = my_ws.char_data->gfx[slot];


	for (i = j = k = 0; sprite[i].tile != -1 || sprite[i].time != -1; i++) {
		h = j * 4;
		if (sprite[i].tile >= 0) {
			d_sprite_hitbox_set(ce->sprite, j, k, cg->sprite_hitbox[h], 
			    cg->sprite_hitbox[h+1], cg->sprite_hitbox[h+2],cg->sprite_hitbox[h+3]);
			d_sprite_frame_entry_set(ce->sprite, j, k++, sprite[i].tile, sprite[i].time);
		} else
			j++, k = 0;
	}

	ce->x = x << 8;
	ce->y = y << 8;
	ce->l = l;
	ce->map = map;
	ce->dx = ce->dy = 0;
	ce->slot = slot;
	ce->dir = 0;
	ce->stat = NULL;
	ce->stats = 0;
	ce->state = NULL;
	ce->item_reply = NULL;
	ce->item_replies = 0;
	ce->char_preload = NULL;
	ce->char_preloads = 0;
	ce->char_effect = NULL;
	ce->char_effects = 0;
	*((unsigned int *) (&ce->special_action)) = 0;

	d_sprite_activate(ce->sprite, 0);
	if (my_ws.char_data->entries == my_ws.char_data->max_entries)
		character_expand_entries();
	for (i = 0; i < my_ws.char_data->max_entries; i++)
		if (!my_ws.char_data->entry[i])
			break;
	my_ws.char_data->entries++;
	my_ws.char_data->entry[i] = ce;
	my_ws.char_data->entry[i]->self = i;
	character_update_sprite(i);

	/* TODO: Implement */
	fprintf(stderr, "TODO: Load savebank %i\n", save);
	ce->save.i = NULL;
	ce->save.b = NULL;
	ce->save.is = ce->save.bs = 0;
	
	ce->loop = character_find_ai_func(ai);
	strcpy(ce->ai, ai);
	ac.msg = AICOMM_MSG_INIT;
	ac.from = -1;
	ac.self = ce->self;
	character_message_loop(ac);
	character_set_hitbox(i);

	return i;
}


void character_spawn_map(int map_slot) {
	int i;
	struct savefile_dungeon_object obj;

	for (i = 0; i < my_ws.dm->grid[map_slot].objects; i++) {
		obj = my_ws.dm->grid[map_slot].object[i];
		
		character_spawn_entry(obj.gfx_slot, obj.ai_func, obj.x * my_ws.camera.tile_w,
			obj.y * my_ws.camera.tile_h, obj.f, my_ws.dm->grid[map_slot].id, obj.save_slot);
	}

	return;
}


void character_loop_entry(struct character_entry *ce) {
	struct aicomm_struct ac;

	ac.msg = AICOMM_MSG_LOOP;
	ac.self = ce->self;
	ac.from = -1;
	character_message_loop(ac);
	character_handle_movement(ce->self);

	return;
}


void character_loop() {
	int i;

	for (i = 0; i < my_ws.char_data->max_entries; i++) {
		if (!my_ws.char_data->entry[i])
			continue;
		character_loop_entry(my_ws.char_data->entry[i]);
	}

	return;
}


void character_despawn(int entry) {
	struct character_entry *ce;
	struct aicomm_struct ac;
	int i;

	if (entry < 0 || entry >= my_ws.char_data->max_entries)
		return;

	ce = my_ws.char_data->entry[entry];
	if (!ce)
		return;
	
	ac.msg = AICOMM_MSG_DESTROY;
	ac.from = -1;
	ac.self = entry;
	character_message_loop(ac);

	for (i = 0; i < ce->char_preloads; i++)
		character_preload_free(ce, ce->char_preload[i].name);
	for (i = ce->char_effects - 1; i >= 0; i--)
		character_effect_free(ce, i);
	
	d_sprite_free(ce->sprite);
	free(ce);
	my_ws.char_data->entry[entry] = NULL;

	if (my_ws.camera.player == entry)
		my_ws.camera.player = -1;

	my_ws.char_data->entries--;

	return;
}


void character_despawn_map(int map) {
	int i;

	for (i = 0; i < my_ws.char_data->entries; i++) {
		if (!my_ws.char_data->entry[i])
			continue;
		if (my_ws.char_data->entry[i]->map == my_ws.dm->grid[map].id)
			character_despawn(i);
	}
	
	return;
}


int character_find_visible() {
	return d_bbox_test(my_ws.char_data->bbox, my_ws.camera.x - 96, my_ws.camera.y - 96,
		my_ws.camera.screen_w + 192, my_ws.camera.screen_h + 192, 
		(unsigned *) my_ws.char_data->collision, my_ws.char_data->max_entries);
}
	

void character_render_layer(int hits, int layer) {
	int i, e;

	for (i = 0; i < hits; i++) {
		e = my_ws.char_data->collision[i];
		if (!my_ws.char_data->entry[e])
			continue;
		if (my_ws.char_data->entry[e]->l != layer)
			continue;
		if (my_ws.char_data->entry[e]->map != my_ws.dm->grid[4].id)
			continue;
		d_sprite_draw(my_ws.char_data->entry[e]->sprite);
	}

	return;
}


void *character_preload_load(const char *resource, enum character_resource type, int tile_w, int tile_h) {
	
	switch (type) {
		case CHARACTER_RES_TILESHEET:
			return d_render_tilesheet_load(resource, tile_w, tile_h, DARNIT_PFORMAT_RGB5A1);
		case CHARACTER_RES_ANIMATION:
			return d_mtsprite_load(resource);
		default:
			fprintf(stderr, "UNIMPLEMENTED RESOURCE TYPE %i\n", type);
			return NULL;
	}
}
	


void character_preload_free(struct character_entry *ce, const char *resource) {
	int i;
	struct char_preload *cr;

	for (i = 0; i < ce->char_preloads; i++)
		if (!strcmp(ce->char_preload[i].name, resource))
			break;
	if (i == ce->char_preloads)
		return;
	cr = &ce->char_preload[i];
	switch (cr->cr) {
		case CHARACTER_RES_TILESHEET:
			d_render_tilesheet_free(cr->resource);
			break;
		case CHARACTER_RES_ANIMATION:
			d_mtsprite_free(cr->resource);
			break;
		default:
			fprintf(stderr, "UNIMPLEMENTED RESOURCE TYPE %i\n", cr->cr);
	}
		
	free(cr->name);

	ce->char_preloads--;
	for (; i < ce->char_preloads; i++)
		ce->char_preload[i] = ce->char_preload[i + 1];

	return;
}


void *character_preload_get(struct character_entry *ce, const char *resource) {
	int i;

	for (i = 0; i < ce->char_preloads; i++)
		if (!strcmp(ce->char_preload[i].name, resource))
			return ce->char_preload[i].resource;
	return NULL;
}


int character_preload_do(struct character_entry *ce, char *fname, enum character_resource type, int loop) {
	int i;

	i = ce->char_preloads;
	ce->char_preloads++;
	ce->char_preload = realloc(ce->char_preload, sizeof(*ce->char_preload) * ce->char_preloads);
	ce->char_preload[i].resource = character_preload_load(fname, type, 0, 0);
	ce->char_preload[i].name = strdup(fname);
	ce->char_preload[i].cr = type;

	return i;
}


int character_effect_new(struct character_entry *ce, char *fname, enum character_effect_t e, int loop) {
	int i;
	void *res;
	enum character_resource type;

	switch (e) {
		case CHARACTER_EFFECT_ANIMATION:
			type = CHARACTER_RES_ANIMATION;
			if (!(res = character_preload_get(ce, fname))) {
				character_preload_do(ce, fname, e, loop);
				res = character_preload_get(ce, fname);
			}

			break;
	}

	for (i = 0; i < ce->char_effects; i++)
		if (!ce->char_effect[i].resource)
			break;
	if (i == ce->char_effects) {
		ce->char_effects++;
		ce->char_effect = realloc(ce->char_effect, ce->char_effects * sizeof(*ce->char_effect));
	}
	
	ce->char_effect[i].loop = loop;
	ce->char_effect[i].cet = type;
	ce->char_effect[i].resource = res;

	return i;
}


void character_effect_free(struct character_entry *ce, int index) {
	switch (ce->char_effect[index].cet) {
		case CHARACTER_EFFECT_ANIMATION:
			/* Do nothing, this is allocated by preload */
			break;
		case CHARACTER_EFFECT_PARTICLE:
			d_particle_free(ce->char_effect[index].resource);
			break;
		default:
			break;
	}

	ce->char_effect[index].resource = NULL;

	return;
}
