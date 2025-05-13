#ifndef __AUTOTILER_H__
#define	__AUTOTILER_H__

#include <darnit_api.h>

struct autotile_tile {
	struct {
		int				neighbour;
		int				start;
		int				mode;
	} *border;
	int				borders;
};


struct autotile {
	struct autotile_tile		*tile;
	int				tiles;
};


struct autotile *autotile_new(DARNIT_STRINGTABLE *st);
void autotile_run(struct autotile *at, unsigned int *src, unsigned int *dst, const int w, const int h);
void *autotile_destroy(struct autotile *at);


#endif
