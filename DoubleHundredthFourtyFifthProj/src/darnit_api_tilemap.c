/*
Copyright (c) 2011-2013 Steven Arnow
'darnit_tilemap.c' - This file is part of libdarnit

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	claim that you wrote the original software. If you use this software
	in a product, an acknowledgment in the product documentation would be
	appreciated but is not required.

	2. Altered source versions must be plainly marked as such, and must not be
	misrepresented as being the original software.

	3. This notice may not be removed or altered from any source
	distribution.
*/


#include "darnit.h"


TILEMAP_ENTRY EXPORT_THIS *d_tilemap_new(unsigned int invisibility_divider, void *tilesheet, unsigned int mask, int w, int h) {
	return tilemapNew(invisibility_divider, tilesheet, mask, w, h, 0);
}


TILEMAP_ENTRY EXPORT_THIS *d_tilemap_isom_new(unsigned int invisibility_divider, void *tilesheet, unsigned int mask, int w, int h, int tile_h) {
	return tilemapNew(invisibility_divider, tilesheet, mask, w, h, tile_h);
}


TILEMAP_ENTRY EXPORT_THIS *d_tilemap_load(const char *fname, unsigned int invisibility_divider, void *tilesheet, unsigned int mask) {
	return tilemapLoad(fname, invisibility_divider, tilesheet, mask, 0);
}


TILEMAP_ENTRY EXPORT_THIS *d_tilemap_isom_load(const char *fname, unsigned int invisibility_divider, void *tilesheet, unsigned int mask, int tile_h) {
	return tilemapLoad(fname, invisibility_divider, tilesheet, mask, tile_h);
}


void EXPORT_THIS d_tilemap_recalc(TILEMAP_ENTRY *tilemap) {
	renderTilemapForceRecalc(tilemap->render);
	
	return;
}


int EXPORT_THIS d_tilemap_sprite_add(TILEMAP_ENTRY *tilemap, SPRITE_ENTRY *sprite) {
	if (!tilemap)
		return -1;
	return renderTilemapSpriteAdd(tilemap->render, sprite);
}


int EXPORT_THIS d_tilemap_sprite_delete(TILEMAP_ENTRY *tilemap, SPRITE_ENTRY *sprite) {
	if (!tilemap)
		return -1;
	return renderTilemapSpriteDelete(tilemap->render, sprite);
}


void EXPORT_THIS d_tilemap_camera_move(TILEMAP_ENTRY *tilemap, int cam_x, int cam_y) {
	if (tilemap == NULL) return;
	renderTilemapCameraMove(tilemap->render, cam_x, cam_y);

	return;
}


void EXPORT_THIS d_tilemap_draw(TILEMAP_ENTRY *tilemap) {
	if (tilemap == NULL) return;
	renderTilemapRender(tilemap->render);

	return;
}


TILEMAP_ENTRY EXPORT_THIS *d_tilemap_free(TILEMAP_ENTRY *tilemap) {
	return tilemapFree(tilemap);
}


void EXPORT_THIS d_tilemap_screen_to_iso(TILEMAP_ENTRY *tm, int x, int y, int *tile_x, int *tile_y) {
	if (tm->render->isometric)
		renderTilemapISOCoordinates(tm->render, x, y, tile_x, tile_y);
	else {
		*tile_x = x / tm->render->ts->wsq;
		*tile_y = y / tm->render->ts->hsq;
	}

	return;
}


void EXPORT_THIS d_tilemap_iso_to_screen(TILEMAP_ENTRY *tm, int tile_x, int tile_y, int *x, int *y) {
	if (tm->render->isometric)
		renderTilemapToISOCoordinates(tm->render, tile_x, tile_y, x, y);
	else {
		*x = tile_x * tm->render->ts->wsq + (tm->render->ts->wsq >> 1);
		*y = tile_y * tm->render->ts->hsq;
	}

	return;
}
