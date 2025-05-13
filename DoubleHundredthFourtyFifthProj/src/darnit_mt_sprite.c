/*
Copyright (c) 2011-2014 Steven Arnow
'mt_sprite.c' - This file is part of libdarnit

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

void mtSpriteCalcCacheTile(TILESHEET *ts, TILE_CACHE *cache, int x, int y, int w, int h, int rx, int ry) {
	renderCalcTilePosCache(cache, ts, rx, ry, 0);
	renderSetTileCoord(cache, ts, x, y, w, h);

	return;
}


void mtSpriteSetAsFrame(MTSPRITE_ENTRY *spr, int time) {
	MTSPRITE_FRAME *tmp;
	if (spr == NULL)
		return;
	
	if ((tmp = realloc(spr->frame, sizeof(MTSPRITE_FRAME) * spr->frames + 1)) == NULL)
		return;
	spr->frame = tmp;
	spr->frame[spr->frames].cache = &spr->cache[spr->tiles - 1];
	spr->frame[spr->frames].tiles = 1;
	spr->frame[spr->frames].time = time;
	
	spr->frames++;

	return;
}


void mtSpriteAddTile(MTSPRITE_ENTRY *spr, int x, int y, int w, int h, int rx, int ry) {
	TILE_CACHE *tmp;
	int i, j;

	if (spr == NULL)
		return;
	
	if ((tmp = realloc(spr->cache, sizeof(TILE_CACHE) * spr->tiles + 1)) == NULL)
		return;
	spr->cache = tmp;
	mtSpriteCalcCacheTile(spr->ts, &spr->cache[spr->tiles], x, y, w, h, rx, ry);

	spr->tiles++;
	for (i = j = 0; i < spr->frames; j += spr->frame[i].tiles, i++)
		spr->frame[i].cache = &spr->cache[j];
	
	return;
}


int mtSpriteEventAdd(MTSPRITE_ENTRY *spr, PARTICLE *p, int frame) {
	spr->frame[frame].event = realloc(spr->frame[frame].event, sizeof(*spr->frame[frame].event) * (spr->frame[frame].events + 1));
	spr->frame[frame].event[spr->frame[frame].events].p = p;
	return spr->frame[frame].events++;
}


void mtSpriteEventRun(MTSPRITE_ENTRY *spr) {
	int i;
	MTSPRITE_PARTICLE_EVENT *ev;

	for (i = 0; i < spr->frame[spr->cur_frame].events; i++) {
		ev = &spr->frame[spr->cur_frame].event[i];
		switch (ev->particle_prop) {
			case MTSPRITE_PARTICLE_EVENT_PULSE:
				ev->p->pulse = 1;
				break;
			case MTSPRITE_PARTICLE_EVENT_MODE:
				ev->p->mode = ev->arg[0];
				break;
			case MTSPRITE_PARTICLE_EVENT_ANGLE:
				ev->p->min_angle = ev->arg[0];
				ev->p->max_angle = ev->arg[1];
				break;
			case MTSPRITE_PARTICLE_EVENT_LIFE:
				ev->p->particle_life = ev->arg[0];
				particleColorDelta(ev->p);
				break;
			case MTSPRITE_PARTICLE_EVENT_SRATE:
				ev->p->spawnrate_max = ev->arg[0];
				break;
			case MTSPRITE_PARTICLE_EVENT_PSIZE:
				ev->p->point_size = ev->arg[0];
				break;
			case MTSPRITE_PARTICLE_EVENT_MOVE:
				ev->p->emit_x = ev->arg[0] * 1000;
				ev->p->emit_y = ev->arg[1] * 1000;
				break;
			case MTSPRITE_PARTICLE_EVENT_VELOCITY:
				ev->p->min_velocity = ev->arg[0];
				ev->p->max_velocity = ev->arg[1];
				break;
			case MTSPRITE_PARTICLE_EVENT_GRAVITY:
				ev->p->gravity_x = ev->arg[0];
				ev->p->gravity_y = ev->arg[1];
				break;
		}
	}

	return;
}


PARTICLE *mtSpriteLoadParticle(MTSPRITE_ENTRY *spr, int type, int max_particles, int top) {
	PARTICLE **p;

	if (top) {
		spr->p.particle_t = realloc(spr->p.particle_t, sizeof(*spr->p.particle_t) * (spr->p.particles_t + 1));
		p = &spr->p.particle_t[spr->p.particles_t++];
	} else {
		spr->p.particle_b = realloc(spr->p.particle_b, sizeof(*spr->p.particle_b) * (spr->p.particles_b + 1));
		p = &spr->p.particle_b[spr->p.particles_b++];
	}

	*p = particleNew(max_particles, type);
	
	return *p;
}


void *mtSpriteNew(int tiles, int frames, void *ts) {
	MTSPRITE_ENTRY *spr;
	int i;

	if ((spr = malloc(sizeof(MTSPRITE_ENTRY))) == NULL)
		return NULL;
	if ((spr->cache = malloc(sizeof(TILE_CACHE) * tiles)) == NULL) {
		free(spr);
		return NULL;
	}
	
	if ((spr->frame = malloc(sizeof(MTSPRITE_FRAME) * frames)) == NULL) {
		free(spr->cache);
		free(spr);
		return NULL;
	}

	spr->frames = frames;
	spr->tiles = tiles;
	spr->cur_frame = 0;
	spr->time_left = 0;
	spr->time_last = 0;
	spr->animate = 0;
	spr->ts = ts;
	spr->p.particle_b = spr->p.particle_t = NULL;
	spr->p.particles_b = spr->p.particles_t = 0;

	for (i = 0; i < spr->frames; i++) {
		spr->frame[i].cache = NULL;
		spr->frame[i].tiles = 0;
		spr->frame[i].events = 0;
		spr->frame[i].event = NULL;
		spr->frame[i].time = 1;
	}

	return spr;
}


void *mtSpriteLoad(const char *fname) {
	MTSPRITE_ENTRY *spr;
	FILESYSTEM_FILE *fp;
	int frames, loctiles, particles, tiles, x, y, w, h, t, rx, ry, ps;
	unsigned int sc, tc, cr, cg, cb, ca;
	char c, buff[512], buf2[128];
	void *ts;
	PARTICLE **p, *pp;

	if ((fp = fsFileOpen(fname, "rb")) == NULL) {
		fprintf(stderr, "libDarnit: Unable to open mt-sprite %s\n", fname);
		return NULL;
	}
	
	ts = NULL;
	frames = tiles = 0;
	while (!fsFileEOF(fp)) {
		fsFileRead((void *) &c, 1, fp);
		switch (c) {
			case 'F':
				fsFileGets(buff, 512, fp);
				frames++;
				break;
			case 'T':
				fsFileGets(buff, 512, fp);
				tiles++;
				break;
			case 'R':	/* ONE resource per file, and it must be the first thing in the file */
				fsFileSkipWhitespace(fp);
				fsFileGets(buff, 512, fp);
				if (buff[strlen(buff) - 1] == '\n')
					buff[strlen(buff) - 1] = 0;
				ts = renderTilesheetLoad(buff, 32, 32, PFORMAT_RGB5A1);
				break;
			case 'P':
				particles++;
			case '\n':
				break;
			default:
				fsFileGets(buff, 512, fp);
				break;
		}
	}
	
	p = calloc(sizeof(*p) * particles, 1);
	ps = 0;

	if ((spr = mtSpriteNew(tiles, frames, ts)) == NULL) {
		fsFileClose(fp);
		free(spr->cache);
		free(spr);
		return NULL;
	}

	fsFileSeek(fp, 0, SEEK_SET);

	/* I know, two-pass parsing is kinda paper-tape era-ish. But I don't know what else to do D: */
	frames = loctiles = tiles = 0;
	while (!fsFileEOF(fp)) {
		fsFileRead(&c, 1, fp);
		switch (c) {
			case 'P':
				fsFileGets(buff, 512, fp);
				/* fname, max_particles, if top or bottom, tile_w, tile_h, tile_id, source color, target color */
				sscanf(buff, "%s %i %i %i %i %i %X %X\n", buf2, &x, &y, &w, &h, &rx, &sc, &tc);
				t = (!strcmp(buf2, "NULL")) ? PARTICLE_TYPE_POINT : PARTICLE_TYPE_TEXTURED;
				p[ps] = mtSpriteLoadParticle(spr, t, x, y);
				if (t == PARTICLE_TYPE_TEXTURED)
					particleSetTexture(p[ps], buf2, w, h, rx);
				else
					p[ps]->point_size = w;
				
				cr = (sc >> 24) & 0xFF;
				cg = (sc >> 16) & 0xFF;
				cb = (sc >> 8)  & 0xFF;
				ca = sc & 0xFF;
				p[ps]->source.r = cr * 1000000, p[ps]->source.g = cg * 1000000;
				p[ps]->source.b = cb * 1000000, p[ps]->source.a = ca * 1000000;
				particleColorDelta(p[ps]);
				cr = (tc >> 24) & 0xFF;
				cg = (tc >> 16) & 0xFF;
				cb = (tc >> 8)  & 0xFF;
				ca = tc & 0xFF;
				p[ps]->target.r = cr * 1000000, p[ps]->target.g = cg * 1000000;
				p[ps]->target.b = cb * 1000000, p[ps]->target.a = ca * 1000000;
				particleColorDelta(p[ps]);
				ps++;
				break;
			case 'E':
				MTSPRITE_PARSE_START(sscanf(buff, "%i %i\n", &x, &y))
				spr->frame[frames-1].event[rx].particle_prop = MTSPRITE_PARTICLE_EVENT_PULSE;
				break;
			case 'M':
				MTSPRITE_PARSE_START(sscanf(buff, "%i %i %i\n", &x, &y, &w))
				spr->frame[frames-1].event[rx].particle_prop = MTSPRITE_PARTICLE_EVENT_MODE;
				if (!w)
					spr->frame[frames-1].event[rx].arg[0] = PARTICLE_MODE_OFF;
				else if (w == 1)
					spr->frame[frames-1].event[rx].arg[0] = PARTICLE_MODE_SHOWER;
				else if (w == 2)
					spr->frame[frames-1].event[rx].arg[0] = PARTICLE_MODE_PULSAR;
				else if (w == 3)
					spr->frame[frames-1].event[rx].arg[0] = PARTICLE_MODE_AUTOPULSAR;
				break;	
			case 'A':
				MTSPRITE_PARSE_START(sscanf(buff, "%i %i %i %i\n", &x, &y, &w, &h))
				spr->frame[frames-1].event[rx].arg[0] = w;
				spr->frame[frames-1].event[rx].arg[1] = h;
				spr->frame[frames-1].event[rx].particle_prop = MTSPRITE_PARTICLE_EVENT_ANGLE;
				break;
			case 'L':
				MTSPRITE_PARSE_START(sscanf(buff, "%i %i %i\n", &x, &y, &w))
				spr->frame[frames-1].event[rx].particle_prop = MTSPRITE_PARTICLE_EVENT_LIFE;
				spr->frame[frames-1].event[rx].arg[0] = w;
				break;
			case 'S':
				MTSPRITE_PARSE_START(sscanf(buff, "%i %i %i\n", &x, &y, &w))
				spr->frame[frames-1].event[rx].particle_prop = MTSPRITE_PARTICLE_EVENT_SRATE;
				spr->frame[frames-1].event[rx].arg[0] = w;
				break;
			case 'Z':
				MTSPRITE_PARSE_START(sscanf(buff, "%i %i %i\n", &x, &y, &w))
				spr->frame[frames-1].event[rx].particle_prop = MTSPRITE_PARTICLE_EVENT_PSIZE;
				spr->frame[frames-1].event[rx].arg[0] = w;
				break;
			case 'C':
				MTSPRITE_PARSE_START(sscanf(buff, "%i %i %i %i\n", &x, &y, &w, &h))
				spr->frame[frames-1].event[rx].arg[0] = w;
				spr->frame[frames-1].event[rx].arg[1] = h;
				spr->frame[frames-1].event[rx].particle_prop = MTSPRITE_PARTICLE_EVENT_MOVE;
				break;
			case 'V':
				MTSPRITE_PARSE_START(sscanf(buff, "%i %i %i %i\n", &x, &y, &w, &h))
				spr->frame[frames-1].event[rx].arg[0] = w;
				spr->frame[frames-1].event[rx].arg[1] = h;
				spr->frame[frames-1].event[rx].particle_prop = MTSPRITE_PARTICLE_EVENT_VELOCITY;
				break;
			case 'G':
				MTSPRITE_PARSE_START(sscanf(buff, "%i %i %i %i\n", &x, &y, &w, &h))
				spr->frame[frames-1].event[rx].arg[0] = w;
				spr->frame[frames-1].event[rx].arg[1] = h;
				spr->frame[frames-1].event[rx].particle_prop = MTSPRITE_PARTICLE_EVENT_GRAVITY;
				break;
			case 'F':
				fsFileGets(buff, 512, fp);
				t = atoi(buff);
				if (frames != 0)
					spr->frame[frames-1].tiles = loctiles;
				loctiles = 0;
				spr->frame[frames].cache = &spr->cache[tiles];
				spr->frame[frames].time = t;
				frames++;
				break;
			case 'T':
				fsFileGets(buff, 512, fp);
				sscanf(buff, "%i %i %i %i %i %i\n", &x, &y, &w, &h, &rx, &ry);
				mtSpriteCalcCacheTile(spr->ts, &spr->cache[tiles], x, y, w, h, rx, ry);
				tiles++;
				loctiles++;
				break;
			case '\n':
				break;
			default:
				fsFileGets(buff, 512, fp);
				break;
		}
	}

	spr->frame[frames-1].tiles = loctiles;
	spr->time_left = spr->frame->time;
	spr->repeat = 1;
	mtSpriteEventRun(spr);

	fsFileClose(fp);
	free(p);

	return spr;
}


void mtSpriteAnimate(MTSPRITE_ENTRY *spr) {
	if (spr == NULL) return;

	int time;

	if (spr->animate == 0)
		return;
	
	time = tpw_ticks();
	time -= spr->time_last;

	spr->time_left -= time;

	while (spr->time_left <= 0) {
		spr->cur_frame++;
		if (spr->cur_frame >= spr->frames)
			spr->cur_frame = (spr->repeat) ? 0 : spr->cur_frame - 1;
		if (spr->repeat || spr->cur_frame < spr->frames)
			spr->time_left += spr->frame[spr->cur_frame].time;
		else {
			spr->time_left = 0;
			break;
		}
		mtSpriteEventRun(spr);
	}

	spr->time_last = tpw_ticks();
	
	return;
}


void mtSpriteDraw(MTSPRITE_ENTRY *spr) {
	int i;
	if (spr == NULL) return;

	mtSpriteAnimate(spr);
	for (i = 0; i < spr->p.particles_b; i++)
		particleLoop(spr->p.particle_b[i]);
	renderCache(spr->frame[spr->cur_frame].cache, spr->ts, spr->frame[spr->cur_frame].tiles);
	for (i = 0; i < spr->p.particles_t; i++)
		particleLoop(spr->p.particle_t[i]);

	return;
}


void mtSpriteEnableAnimation(MTSPRITE_ENTRY *spr) {
	if (spr == NULL) return;

	spr->time_last = tpw_ticks();
	spr->animate = 1;

	return;
}


void mtSpritePauseAnimation(MTSPRITE_ENTRY *spr) {
	if (spr == NULL) return;

	spr->animate = 0;

	return;
}


void mtSpriteDisableAnimation(MTSPRITE_ENTRY *spr) {
	if (spr == NULL) return;

	spr->animate = 0;
	spr->cur_frame = 0;
	spr->time_left = spr->frame[spr->cur_frame].time;
	
	return;
}


void mtSpriteSetRepeat(MTSPRITE_ENTRY *spr, int repeat) {
	if (!spr)
		return;
	spr->repeat = (repeat) ? 1 : 0;
	
	return;
}


void *mtSpriteDelete(MTSPRITE_ENTRY *spr) {
	int i;
	if (spr == NULL) return NULL;
	
	spr->ts = renderTilesheetFree(spr->ts);

	for (i = 0; i < spr->frames; i++)
		free(spr->frame[i].event);

	free(spr->cache);
	free(spr->frame);
	free(spr);

	for (i = 0; i < spr->p.particles_b; i++)
		particleFree(spr->p.particle_b[i]);
	free(spr->p.particle_b);
	for (i = 0; i < spr->p.particles_t; i++)
		particleFree(spr->p.particle_t[i]);
	free(spr->p.particle_t);

	return NULL;
}

// DARNIT_HEADLESS
#endif
