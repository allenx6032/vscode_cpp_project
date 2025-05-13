/*
Copyright (c) 2013, 2014 Steven Arnow <s@rdw.se>
'tbbox.c' - This file is part of libdarnit

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

#define	MIN(x, y)		((x) < (y) ? (x) : (y))
#define	MAX(x, y)		((x) > (y) ? (x) : (y))
#define	MIN3(x, y, z)		(MIN(MIN(x, y), z))
#define	MAX3(x, y, z)		(MAX(MAX(x, y), z))
#define	X(i)			(tb->entry[i].pos_low_x)
#define	Y(i)			(tb->entry[i].pos_low_y)


TBBOX *tbboxNew(int size, enum tbbox_sortmode mode) {
	TBBOX *tb;
	int i;

	if (!(tb = malloc(sizeof(*tb))))
		return tb;
	tb->used = 0;
	tb->entries = size;
	tb->entry = malloc(sizeof(*tb->entry) * size);
	tb->lookup = malloc(sizeof(*tb->lookup) * size);
	tb->mode = mode;

	for (i = 0; i < size; i++)
		tb->entry[i].mode = TBBOX_MODE_NOT_USED, tb->entry[i].id = i, tb->lookup[i] = -1;
	
	return tb;
}


TBBOX *tbboxFree(TBBOX *tb) {
	if (!tb)
		return NULL;
	
	free(tb->entry);
	free(tb->lookup);
	free(tb);

	return NULL;
}


int tbboxFindFreeSlot(TBBOX *tb) {
	int i;

	for (i = 0; i < tb->entries; i++)
		if (tb->lookup[i] < 0)
			return i;
	return -1;
}


static void tbboxSortSwitch(TBBOX *tb, int i) {
	TBBOX_ENTRY tmpe;
	int tmpi;

	tmpi = tb->lookup[i];
	tb->lookup[i] = tb->lookup[i-1];
	tb->lookup[i-1] = tmpi;
	tmpe = tb->entry[i];
	tb->entry[i] = tb->entry[i-1];
	tb->entry[i-1] = tmpe;
	return;
}


void tbboxSort(TBBOX *tb) {
	int i, j, sort;
	
	if (!tb)
		return;
	/* Just re-using the same insertion-sort from bbox */
	for (sort = 0, i = 1; i < tb->entries; i++, sort = 0) {
		for (j = i; j > 0; j--, sort = 0) {
			if (tb->lookup[j - 1] < 0)
				sort = 1;
			if (tb->mode == SORT_MODE_X) {
				if (X(j) < X(j-1) || (X(j) == X(j-1) && Y(j) < Y(j-1)))
					sort = 1;
			} else {
				if (Y(j) < Y(j-1) || (Y(j) == Y(j-1) && X(j) < X(j-1)))
					sort = 1;
			}
		
			if (!sort)
				break;
			tbboxSortSwitch(tb, j);
		}
	}

	tb->sort = 0;
	return;
}


int tbboxAddCircle(TBBOX *tb, int x, int y, int radius) {
	int key;

	if (!tb) return -1;
	if ((key = tbboxFindFreeSlot(tb)) < 0)
		return -1;
	tb->used++;
	tb->entry[key].mode = TBBOX_MODE_CIRCLE;
	tb->entry[key].pos_low_x = x - radius;
	tb->entry[key].pos_low_y = y - radius;
	tb->entry[key].pos_high_x = x + radius;
	tb->entry[key].pos_high_y = y + radius;

	tb->entry[key].cent.point_x = x;
	tb->entry[key].cent.point_y = y;
	tb->entry[key].cent.radius = radius;

	tb->lookup[tb->entry[key].id] = key;
	tb->sort = 1;

	return tb->entry[key].id;
}


int tbboxAddTriangleBox(TBBOX *tb, int x, int y, int x1, int y1, int x2, int y2, int x3, int y3) {
	int key, l, h;

	if (!tb) return -1;
	if ((key = tbboxFindFreeSlot(tb)) < 0)
		return -1;
	tb->used++;
	tb->entry[key].mode = TBBOX_MODE_TRIANGLE;
	l = MIN3(x1, x2, x3);
	h = MAX3(x1, x2, x3);
	tb->entry[key].pos_low_x = l;
	tb->entry[key].pos_high_x = h;
	l = MIN3(y1, y2, y3);
	h = MAX3(y1, y2, y3);
	tb->entry[key].pos_low_y = l;
	tb->entry[key].pos_high_y = h;

	tb->entry[key].cent.point_x = x;
	tb->entry[key].cent.point_y = y;

	tb->lookup[tb->entry[key].id] = key;
	tb->sort = 1;

	return tb->entry[key].id;
}


static int tbboxTestCircleCircle(TBBOX *tb, int entry, int x, int y, int radius) {
	int dx, dy, r, fred;

	dx = tb->entry[entry].cent.point_x - x;
	dy = tb->entry[entry].cent.point_y - y;
	r = dx * dx + dy * dy;
	fred = tb->entry[entry].cent.radius + radius;
	return fred * fred >= r;
}
