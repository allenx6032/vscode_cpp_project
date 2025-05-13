#include <darnit_api.h>
#include <math.h>

#include "camera.h"
#include "world.h"
#include "save_loader.h"
#include "character.h"

void camera_init() {
	DARNIT_PLATFORM p;

	p = d_platform_get();
	my_ws.camera.x = my_ws.camera.y = 0;
	my_ws.camera.follow_char = -1;

	my_ws.camera.screen_w = p.screen_w;
	my_ws.camera.screen_h = p.screen_h;
	my_ws.camera.player = -1;
	my_ws.camera.tile_w = TILE_W;
	my_ws.camera.tile_h = TILE_H;
	my_ws.camera.scroll_speed = 800;		/* 400 pixels / second */
	my_ws.camera.jump = 0;

	return;
}


void camera_loop() {
	int x, y, w, h, dx, dy, s2, t, f;

	if (my_ws.camera.follow_char < 0)
		return;
	if (my_ws.camera.follow_char >= my_ws.char_data->max_entries)
		return;
	if (!my_ws.char_data->entry[my_ws.camera.follow_char]) {
		my_ws.camera.follow_char = -1;
		return;
	}

	d_sprite_hitbox(my_ws.char_data->entry[my_ws.camera.follow_char]->sprite, &x, &y, &w, &h);
	x -= (my_ws.camera.screen_w >> 1);
	y -= (my_ws.camera.screen_h >> 1);
	x += (w >> 1);
	y += (h >> 1);
	x += (my_ws.char_data->entry[my_ws.camera.follow_char]->x >> 8);
	y += (my_ws.char_data->entry[my_ws.camera.follow_char]->y >> 8);
	dx = x - my_ws.camera.x;
	dy = y - my_ws.camera.y;

	t = my_ws.camera.scroll_speed * d_last_frame_time();
	t /= 1000;
	s2 = t * t;
	if (dx * dx + dy * dy > s2 && !my_ws.camera.jump) {
		f = ((int) sqrtf(dx * dx + dy * dy)) << 8;
		t = f / (sqrtf(s2));
		dx = (dx << 8) / t;
		dy = (dy << 8) / t;
		my_ws.camera.x += dx;
		my_ws.camera.y += dy;
	} else {
		my_ws.camera.jump = 0;
		my_ws.camera.x = x;
		my_ws.camera.y = y;
	}

	w = (*my_ws.dm->grid[4].layer)->w * my_ws.camera.tile_w;
	h = (*my_ws.dm->grid[4].layer)->h * my_ws.camera.tile_h;

	
	if (my_ws.camera.screen_w < w) {
		if (my_ws.camera.x + my_ws.camera.screen_w > w)
			my_ws.camera.x = w - my_ws.camera.screen_w;
		else if (my_ws.camera.x < 0)
			my_ws.camera.x = 0;
	} else
		my_ws.camera.x = (my_ws.camera.screen_w - w) / 2;

	if (my_ws.camera.screen_h < h) {
		if (my_ws.camera.y + my_ws.camera.screen_h > h)
			my_ws.camera.y = h - my_ws.camera.screen_h;
		else if (my_ws.camera.y < 0)
			my_ws.camera.y = 0;
	} else
		my_ws.camera.y = (my_ws.camera.screen_h - h) / 2;
	
	return;
}
