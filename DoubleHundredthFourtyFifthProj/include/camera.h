#ifndef __CAMERA_H__
#define	__CAMERA_H__

#include <darnit_api.h>

struct camera {
	int			x;
	int			y;
	int			follow_char;
	int			screen_w;
	int			screen_h;
	int			player;
	int			tile_w;
	int			tile_h;
	int			scroll_speed;
	int			jump;
	DARNIT_TILESHEET	*sys;
};


void camera_init();
void camera_loop();

#endif
