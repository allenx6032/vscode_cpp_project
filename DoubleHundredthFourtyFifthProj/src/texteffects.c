#include "world.h"
#include <string.h>


void texteffect_init() {
	my_ws.te.te = NULL;
	my_ws.te.max_te = 0;
	my_ws.te.te_used = 0;
}


void texteffect_done() {
	unsigned i;

	for (i = 0; i < my_ws.te.max_te; i++)
		if (my_ws.te.te[i].time_left > 0)
			my_ws.te.te[i].s = d_text_surface_free(my_ws.te.te[i].s);
	
	free(my_ws.te.te);
	my_ws.te.te = NULL;
	my_ws.te.max_te = 0;
	my_ws.te.te_used = 0;

	return;
}


void texteffect_expand() {
	unsigned i;

	i = my_ws.te.max_te;
	my_ws.te.max_te += 16;
	my_ws.te.te = realloc(my_ws.te.te, sizeof(*my_ws.te.te) * my_ws.te.max_te);
	for (; i < my_ws.te.max_te; i++) {
		my_ws.te.te[i].time_left = 0;
		my_ws.te.te[i].s = NULL;
	}

	return;
}


int texteffect_add(const char *str, int time, int x, int y, int linel, unsigned r, unsigned g, unsigned b) {
	unsigned i;
	int j;

	if (my_ws.te.max_te == my_ws.te.te_used)
		texteffect_expand();
	for (i = 0; i < my_ws.te.max_te; i++) {
		if (my_ws.te.te[i].time_left)
			continue;
		else
			break;
	}

	j = 0;
	y -= d_font_string_geometrics_o(my_ws.te.font, str, linel, &j);
	my_ws.te.te[i].s = d_text_surface_color_new(my_ws.te.font, strlen(str), linel, x - (j >> 1), y);
	my_ws.te.te[i].time_left = time;
	my_ws.te.te[i].total_time = time;
	d_text_surface_color_next(my_ws.te.te[i].s, r, g, b);
	d_text_surface_string_append(my_ws.te.te[i].s, str);
	my_ws.te.te_used++;

	return i;
}


void texteffect_del(int id) {
	my_ws.te.te[id].s = d_text_surface_free(my_ws.te.te[id].s);
	my_ws.te.te[id].time_left = 0;

	return;
}


void texteffect_loop() {
	unsigned i, j;
	int k;

	for (i = j = 0; i < my_ws.te.max_te && j < my_ws.te.te_used; i++) {
		if (!my_ws.te.te[i].time_left)
			continue;
		j++;
		k = (my_ws.te.te[i].time_left << 8) / my_ws.te.te[i].total_time;
		k = (k < 0) ? 0 : k;
		k = (k > 255) ? 255 : k;
		d_render_offset(my_ws.camera.x, my_ws.camera.y - (k >> 2));
		d_render_tint(255, 255, 255, k);
		d_text_surface_draw(my_ws.te.te[i].s);
		my_ws.te.te[i].time_left -= d_last_frame_time();
		if (my_ws.te.te[i].time_left <= 0)
			texteffect_del(i);
	}

	d_render_tint(255, 255, 255, 255);
	
	return;
}

	
