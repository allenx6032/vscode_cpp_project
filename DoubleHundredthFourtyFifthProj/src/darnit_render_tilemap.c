/*
Copyright (c) 2011-2013 Steven Arnow
'render_tilemap.c' - This file is part of libdarnit

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

#ifndef DARNIT_HEADLESS
#define	abs(x)		((x) < 0 ? (x * -1) : (x))


void renderTilemapISOCoordinates(RENDER_TILEMAP *tm, int x, int y, int *r_x, int *r_y) {
	isometricFromScreen(x, y, 0, 0, tm->ts->wsq, tm->r_h, r_x, r_y);

	return;
}


void renderTilemapToISOCoordinates(RENDER_TILEMAP *tm, int x, int y, int *r_x, int *r_y) {
	isometricToScreen(x, y, 0, 0, tm->ts->wsq, tm->r_h, r_x, r_y);

	return;
}


void renderTilemapSortSprites(RENDER_TILEMAP *tm) {
	SPRITE_ENTRY *tmp;
	int i, j;
	
	// loopen börjar på 1 för att första elementet är definitivt sorterat när det står ensamt.
	// därefter hämtas ett element i taget ur "buffern" (resten av arrayen)
	for (i = 1; i < tm->sprites_used; i++) {
		// så länge nuvarande bufferelement (j) är mindre än det precis till vänster i den sorterade listan
		// ska det skiftas ett steg till vänster.
		for (j = i; j > 0 && tm->sprite[j]->y < tm->sprite[j-1]->y; j--) {
			tmp = tm->sprite[j];
			tm->sprite[j] = tm->sprite[j-1];
			tm->sprite[j-1] = tmp;
		}
	}
	
	return;
}


int renderTilemapSpriteIndex(RENDER_TILEMAP *tm, SPRITE_ENTRY *sprite) {
	int i;

	for (i = 0; i < tm->sprites_used; i++)
		if (tm->sprite[i] == sprite)
			return i;
	return -1;
}


int renderTilemapSpriteAdd(RENDER_TILEMAP *tm, SPRITE_ENTRY *sprite) {
	SPRITE_ENTRY **tmp;

	if (!sprite)
		return -1;

	if (tm->sprites == tm->sprites_used) {
		tm->sprites++;
		if (!(tmp = realloc(tm->sprite, sizeof(SPRITE_ENTRY *) * tm->sprites))) {
			tm->sprites--;
			return -1;
		}

	}

	tm->sprite[tm->sprites_used] = sprite;
	tm->sprites_used++;

	return tm->sprites_used - 1;
}


int renderTilemapSpriteDelete(RENDER_TILEMAP *tm, SPRITE_ENTRY *sprite) {
	int index;

	if ((index = renderTilemapSpriteIndex(tm, sprite)) < 0)
		return -1;
	tm->sprites_used--;
	tm->sprite[index] = tm->sprite[tm->sprites_used];

	return index;
}


void renderTilemapSpriteSort(SPRITE_ENTRY **arr, int size) {
	SPRITE_ENTRY *tmp;
	int i, j;

	for (i = 0; i < size; i++)
		for (j = i; j > 0 && arr[j]->x < arr[j-1]->x; j--) {
			tmp = arr[j];
			arr[j] = arr[j-1];
			arr[j-1] = tmp;
		}
	return;
}


int renderTilemapFixSprites(RENDER_TILEMAP *tm) {
	int o_x, o_y, x_last, y_start_pix, t, tt, i, sprite_cur, sprite_total;

	if (!tm->isometric)
		return 0;
	
	renderTilemapISOCoordinates(tm, tm->cam_xp - tm->ts->wsq, tm->cam_yp - tm->r_h, &o_x, &o_y);
	renderTilemapToISOCoordinates(tm, o_x, o_y, &x_last, &y_start_pix);
	y_start_pix += tm->r_h;
	sprite_cur = sprite_total = 0;
	
	for (i = 0; i < tm->h && tm->sprite_row; i++) {
		while (sprite_cur < tm->sprites_used) {
			if (((int)(tm->sprite[sprite_cur]->y + tm->sprite[sprite_cur]->ts->hsq)) >= 0) {
				t = RENDER_TILEMAP_PIX_POS(tm, y_start_pix, i) + 1;
				tt = tm->sprite[sprite_cur]->y + tm->sprite[sprite_cur]->ts->hsq;
				if (t < tt)
					break;
				t = RENDER_TILEMAP_PIX_BASE(tm, y_start_pix, i) + 1;
				if (t < tt)
					break;
			}

			sprite_cur++;
		}
		
		tm->sprite_row[i] = !i ? sprite_cur : sprite_cur - sprite_total;
		renderTilemapSpriteSort(&tm->sprite[(i > 0) ? tm->sprite_row[i - 1] : 0], tm->sprite_row[i] - ((i > 0) ? tm->sprite_row[i - 1] : 0));
		sprite_total = sprite_cur;
	}

	return 0;
}
	


int renderTilemapRecalcISO(RENDER_TILEMAP *tm) {
	float x_start, y_start, x_start2, x_pos, y_pos;
	float x_step, y_step;
	int i, j, k, t, o_x, o_y, x_last, y_last, x_cur, y_cur, tile_total;

	x_step = d->video.swgran * tm->ts->wsq;
	y_step = d->video.shgran * tm->r_h / 2;
	tile_total = 0;
	
	renderTilemapToISOCoordinates(tm, 0, 0, &x_last, &y_last);

	renderTilemapISOCoordinates(tm, tm->cam_xp - tm->ts->wsq, tm->cam_yp - tm->r_h, &o_x, &o_y);
	renderTilemapToISOCoordinates(tm, o_x, o_y, &x_last, &y_last);
	x_start = d->video.swgran * (x_last - tm->cam_xp) - 1.0f - x_step / 2;
	y_start = 1.0f - d->video.shgran * (y_last - tm->cam_yp) - 2 * y_step;

	renderTilemapToISOCoordinates(tm, o_x - 1, o_y, &x_last, &y_last);
	x_start2 = d->video.swgran * (x_last - tm->cam_xp) - 1.0f - x_step / 2;

	y_pos = y_start;
	for (i = k = 0; i < tm->h; i++, y_pos -= y_step) {
		x_pos = (i & 1) ? x_start2 : x_start;
		x_cur = o_x + (i >> 1);
		y_cur = o_y + (i >> 1);
		y_cur += (i & 1);

		for (j = 0; j < tm->w; j++, x_cur++, y_cur--, x_pos += x_step) {
			if (x_cur >= tm->map_w)
				break;
			if (x_cur < 0)
				continue;
			if (y_cur < 0)
				break;
			if (y_cur >= tm->map_h)
				continue;

			t = tm->map[x_cur + y_cur * tm->map_w] & tm->mask;
			if (t >= tm->ts->tiles)
				continue;

			if (tm->inv_div != 0)
				if (t % tm->inv_div == 0)
					continue;

			/* Rurgh... */
			RENDER_TILEMAP_FILL(k, x_pos, y_pos, x_step, tm->ts->tile[t].h_p, t);
			k++;
		}
		
		tm->tile_row[i] = !i ? k : k - tile_total;
		tile_total = k;
	}

	return k;
}


int renderTilemapRecalc(TILE_CACHE *cache, TILESHEET *ts, int x, int y, int w, int h, int map_w, int map_h, unsigned int *tilemap, int inv_div, unsigned int mask) {
	float x_start, y_start;
	float *x_adv_buf, *y_adv_buf;
	int i, j, k, t, x_cur, y_cur;

	x_adv_buf = malloc(sizeof(float) * (w + 1));
	y_adv_buf = malloc(sizeof(float) * (h + 1));
	x_start = -1.0f;
	y_start = 1.0f;

	for (i = 0; i < (w + 1); i++)
		x_adv_buf[i] = x_start + ts->sw * (float) i;
	for (i = 0; i < (h + 1); i++)
		y_adv_buf[i] = y_start - ts->sh * (float) i;
	
	for (i = k = 0, x_cur = x; i < w; i++, x_cur++) {
		if (x_cur < 0 || x_cur >= map_w)
			continue;
		for (j = 0, y_cur = y; j < h; j++, y_cur++) {
			if (y_cur < 0 || y_cur >= map_h)
				continue;
			t = tilemap[x_cur + y_cur * map_w] & mask;
			if (t >= ts->tiles)
				continue;

			if (inv_div != 0)
				if (t % inv_div == 0)
					continue;
			/* ugh.. */
			cache[k].vertex[0].coord.x = x_adv_buf[i];
			cache[k].vertex[0].coord.y = y_adv_buf[j];
			cache[k].vertex[1].coord.x = x_adv_buf[i+1];
			cache[k].vertex[1].coord.y = y_adv_buf[j];
			cache[k].vertex[2].coord.x = x_adv_buf[i+1];
			cache[k].vertex[2].coord.y = y_adv_buf[j+1];
			cache[k].vertex[3].coord.x = x_adv_buf[i+1];
			cache[k].vertex[3].coord.y = y_adv_buf[j+1];
			cache[k].vertex[4].coord.x = x_adv_buf[i];
			cache[k].vertex[4].coord.y = y_adv_buf[j+1];
			cache[k].vertex[5].coord.x = x_adv_buf[i];
			cache[k].vertex[5].coord.y = y_adv_buf[j];
			cache[k].vertex[0].tex.u = ts->tile[t].r;
			cache[k].vertex[0].tex.v = ts->tile[t].s;
			cache[k].vertex[1].tex.u = ts->tile[t].u;
			cache[k].vertex[1].tex.v = ts->tile[t].s;
			cache[k].vertex[2].tex.u = ts->tile[t].u;
			cache[k].vertex[2].tex.v = ts->tile[t].v;
			cache[k].vertex[3].tex.u = ts->tile[t].u;
			cache[k].vertex[3].tex.v = ts->tile[t].v;
			cache[k].vertex[4].tex.u = ts->tile[t].r;
			cache[k].vertex[4].tex.v = ts->tile[t].v;
			cache[k].vertex[5].tex.u = ts->tile[t].r;
			cache[k].vertex[5].tex.v = ts->tile[t].s;
			k++;
		}
	}

	free(x_adv_buf);
	free(y_adv_buf);
	
	return k;
}


void renderTilemapCameraMove(RENDER_TILEMAP *tm, int cam_x, int cam_y) {
	int x, y, w, h, map_w, map_h;
	int off_x, off_y;

	off_x = (cam_x < 0) ? tm->ts->wsq - (abs(cam_x) % tm->ts->wsq) : (cam_x % tm->ts->wsq);
	off_y = (cam_y < 0) ? tm->ts->hsq - (abs(cam_y) % tm->r_h) : (cam_y % tm->r_h);

	tm->cam_x = d->video.swgran * off_x * -1;
	tm->cam_y = d->video.shgran * off_y;

	tm->cam_xi_c = cam_x;
	tm->cam_yi_c = cam_y;


	if (cam_x < 0 && off_x == tm->ts->wsq)
		cam_x -= tm->ts->wsq;
	if (cam_y < 0 && off_y == tm->ts->hsq)
		cam_y -= tm->ts->hsq;
	x = floorf((float) cam_x / tm->ts->wsq);
	y = floorf((float) cam_y / tm->r_h);

	if (x == tm->cam_xi && y == tm->cam_yi)
		return;

	w = tm->w;
	h = tm->h;
	map_w = tm->map_w;
	map_h = tm->map_h;


	if (tm->isometric) {
		tm->cam_xp = cam_x / tm->ts->wsq * tm->ts->wsq;
		tm->cam_yp = cam_y / tm->r_h * tm->r_h;
		if (cam_y < 0 && cam_y % tm->r_h)
			tm->cam_yp -= tm->r_h;
		tm->cache_used = renderTilemapRecalcISO(tm);
	} else
		tm->cache_used = renderTilemapRecalc(tm->cache, tm->ts, x, y, w, h, map_w, map_h, tm->map, tm->inv_div, tm->mask);

	tm->cam_xi = x;
	tm->cam_yi = y;
	
	return;
}


void *renderTilemapCreateISO(unsigned int w, unsigned int h, unsigned int *map, int camera_x, int camera_y, unsigned int inv_div, TILESHEET *ts, unsigned int mask, int r_h) {
	RENDER_TILEMAP *tm;
	
	if ((tm = malloc(sizeof(RENDER_TILEMAP))) == NULL) {
		return NULL;
	}

	tm->map_w = w;
	tm->map_h = h;
	tm->cam_xi = ~0;
	tm->cam_yi = ~0;
	tm->map = map;
	tm->inv_div = inv_div;
	tm->ts = ts;
	tm->w = d->video.w / ts->wsq + 4;
	tm->r_h = r_h;
	tm->h = d->video.h / r_h * 2 + 3 + tm->ts->hsq / tm->r_h * 2;
	tm->mask = mask;
	tm->isometric = 1;
	tm->cam_xp = 0;
	tm->cam_yp = 0;
	tm->tile_row = calloc(tm->h, sizeof(unsigned int));

	tm->sprite = calloc(TILEMAP_SPRITE_START, sizeof(SPRITE_ENTRY *));
	tm->sprite_row = calloc(tm->h, sizeof(unsigned int));
	tm->sprites = TILEMAP_SPRITE_START;
	tm->sprites_used = 0;
	
	tm->cache = malloc(sizeof(TILE_CACHE) * tm->w * tm->h);
	renderTilemapCameraMove(tm, camera_x, camera_y);
	renderTilemapForceRecalc(tm);

	return tm;
}


void *renderTilemapCreate(unsigned int w, unsigned int h, unsigned int *map, int camera_x, int camera_y, unsigned int inv_div, TILESHEET *ts, unsigned int mask) {
	RENDER_TILEMAP *tm;
	
	if ((tm = malloc(sizeof(RENDER_TILEMAP))) == NULL) {
		return NULL;
	}

	tm->map_w = w;
	tm->map_h = h;
	tm->cam_xi = ~0;
	tm->cam_yi = ~0;
	tm->map = map;
	tm->inv_div = inv_div;
	tm->ts = ts;
	tm->w = d->video.w / ts->wsq + 2;
	tm->h = d->video.h / ts->hsq + 2;
	tm->mask = mask;
	tm->isometric = 0;
	tm->r_h = ts->hsq;

	tm->sprite = calloc(TILEMAP_SPRITE_START, sizeof(SPRITE_ENTRY *));
	tm->sprites = TILEMAP_SPRITE_START;
	tm->sprites_used = 0;

	tm->cache = malloc(sizeof(TILE_CACHE) * tm->w * tm->h);
	renderTilemapCameraMove(tm, camera_x, camera_y);
	renderTilemapForceRecalc(tm);

	return tm;
}


void renderTilemapRender(RENDER_TILEMAP *tm) {
	int i, j, s_i, t_i;

	renderTilemapSortSprites(tm);
	renderTilemapFixSprites(tm);
	glLoadIdentity();
	glTranslatef(tm->cam_x, tm->cam_y, 0);
	if (!tm->sprites_used || !tm->isometric)
		renderCache(tm->cache, tm->ts, tm->cache_used);
	else 
		for (i = s_i = t_i = 0; i < tm->h; i++) {
			if (tm->sprite_row[i]) {
				glLoadIdentity();
				glTranslatef(d->video.swgran * tm->cam_xi_c * -1, d->video.shgran * tm->cam_yi_c, 0);
				for (j = 0; j < tm->sprite_row[i]; j++, s_i++)
					spriteDraw(tm->sprite[s_i]);
				glLoadIdentity();
				glTranslatef(tm->cam_x, tm->cam_y, 0);
			}
			if (tm->tile_row[i])
				renderCache(&tm->cache[t_i], tm->ts, tm->tile_row[i]);
			t_i += tm->tile_row[i];

		}


	if (!tm->isometric)
		for (i = 0; i < tm->sprites_used; i++)
			spriteDraw(tm->sprite[i]);
	glLoadIdentity();
	glTranslatef(d->video.swgran * d->video.offset_x, d->video.shgran * d->video.offset_y, 0.0f);

	return;
}


void renderTilemapFree(RENDER_TILEMAP *tm) {
	free(tm->cache);
	free(tm->sprite);
	free(tm);

	return;
}


void renderTilemapForceRecalc(RENDER_TILEMAP *tm) {
	if (tm == NULL) return;

	if (tm->isometric)
		tm->cache_used = renderTilemapRecalcISO(tm);
	else
		tm->cache_used = renderTilemapRecalc(tm->cache, tm->ts, tm->cam_xi, tm->cam_yi, tm->w, tm->h, tm->map_w, tm->map_h, tm->map, tm->inv_div, tm->mask);

	return;
}


// DARNIT_HEADLESS
#endif
