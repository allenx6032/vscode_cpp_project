#include "world.h"


void ambiance_init() {
	my_ws.ambiance.flicker.rect = d_render_rect_new(1);
	d_render_rect_move(my_ws.ambiance.flicker.rect, 0, -1, -1, my_ws.camera.screen_w + 1, my_ws.camera.screen_h + 1);
	my_ws.ambiance.flicker.r = my_ws.ambiance.flicker.g = my_ws.ambiance.flicker.b = 255;
	my_ws.ambiance.flicker.a = 0;
	my_ws.ambiance.flicker.filter_active = 0;
	my_ws.ambiance.flicker.flicker_interval = 0;

	return;
}


void ambiance_draw() {
	int flicker_offset;

	if (my_ws.ambiance.flicker.flicker_interval) {
		if (my_ws.ambiance.flicker.flicker) {
			my_ws.ambiance.flicker.flicker += d_last_frame_time();
			flicker_offset = my_ws.ambiance.flicker.flicker;
			if (flicker_offset >= my_ws.ambiance.flicker.flicker_duration / 2)
				flicker_offset = my_ws.ambiance.flicker.flicker_duration / 2 - flicker_offset;
			flicker_offset *= my_ws.ambiance.flicker.flicker_offset;
			flicker_offset /= my_ws.ambiance.flicker.flicker_duration;
			my_ws.ambiance.flicker.flicker = flicker_offset;
		}
	}
	if (my_ws.ambiance.flicker.filter_active) {
		d_render_offset(0, 0);
		d_render_tint(my_ws.ambiance.flicker.r, my_ws.ambiance.flicker.g, my_ws.ambiance.flicker.b, my_ws.ambiance.flicker.a + my_ws.ambiance.flicker.flicker);
		d_render_rect_draw(my_ws.ambiance.flicker.rect, 1);
	}

	return;
}
