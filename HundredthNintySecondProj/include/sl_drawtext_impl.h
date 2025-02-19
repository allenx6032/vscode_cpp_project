/*
libdrawtext - a simple library for fast text rendering in OpenGL
Copyright (C) 2011-2012  John Tsiombikas <nuclear@member.fsf.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifdef _MSC_VER
	#pragma once
#else
	#ifndef SL_DRAWTEXT_IMPL_H
	#define SL_DRAWTEXT_IMPL_H
#endif

struct glyph {
	int code;
	float x, y, width, height;
	/* normalized coords [0, 1] */
	float nx, ny, nwidth, nheight;
	float orig_x, orig_y;
	float advance;
	struct glyph *next;
};

struct dtx_glyphmap {
	int ptsize;

	int xsz, ysz;
	unsigned char *pixels;
	unsigned int tex;

	int cstart, cend;	/* character range */
	int crange;

	float line_advance;

	struct glyph *glyphs;
	struct dtx_glyphmap *next;
};

// refers to a particular type of font
struct dtx_font {
	// freetype FT_Face
	void *face;

	// active glyphmap
	struct dtx_glyphmap *gmaps;

	// cache of the last active glyphmap
	struct dtx_glyphmap *last_gmap;
};

int dtx_interpolation;
struct dtx_font *dtx_font;
int dtx_font_sz;


#define fperror(str) \
	fprintf(stderr, "%s: %s: %s\n", __FUNCTION__, (str), strerror(errno))

#ifdef _MSC_VER
#else
	#endif
#endif
