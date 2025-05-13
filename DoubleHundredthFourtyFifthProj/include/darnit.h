/*
Copyright (c) 2011-2013 Steven Arnow
'darnit.h' - This file is part of libdarnit

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


#ifndef __DARNIT_H__
#define __DARNIT_H__


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>


#ifdef _WIN32
	#define TARGET_IS_RETARDED
	#include <winsock2.h>
	#include <windows.h>
#else
	#include <dlfcn.h>
	#include <unistd.h>
	#include <netdb.h>
	#include <sys/types.h>
	#include <fcntl.h>
#endif

#include "darnit_tpw.h"

#define		DARNIT_UNUSED(x) (void)x

#define		DARNIT_PLATFORM_DESKTOP		0x100000
#define		DARNIT_PLATFORM_HANDHELD	0x200000

#define		DARNIT_PLATFORM_LINUX		0x1
#define		DARNIT_PLATFORM_WIN32		0x2
#define		DARNIT_PLATFORM_PANDORA		0x4
#define		DARNIT_PLATFORM_GCWZERO		0x8
#define		DARNIT_PLATFORM_GPHCAANOO	0x10
#define		DARNIT_PLATFORM_MACOSX		0x20
#define		DARNIT_PLATFORM_FREEBSD		0x40
#define		DARNIT_PLATFORM_MAEMO		0x80
#define		DARNIT_PLATFORM_DBPYRA		0x100
#define		DARNIT_PLATFORM_BIGENDIAN	0x80000000


#ifdef _WIN32
	// #define EXPORT_THIS __declspec(dllexport)
	#define EXPORT_THIS 
	#define	off_t				long long int
#else
	#define	EXPORT_THIS __attribute__ ((visibility ("default")))
#endif


typedef union {
	unsigned int	i;
	unsigned char	c[4];
} DARNIT_ENDIAN_CONVERT;


typedef struct {
	unsigned int	x	: 16;
	unsigned int	y	: 16;
	signed int	wheel	: 32;
	unsigned int	mb_l	: 1;
	unsigned int	mb_r	: 1;
} DARNIT_MOUSE;



// #include "stb_vorbis.c.inc"
// #include "stb_image.c.inc"
// #include "stb_truetype.c.inc"

// #define STB_IMAGE_IMPLEMENTATION
// #include <stb_image.h>
// #define STB_TRUETYPE_IMPLEMENTATION
// #include <stb_truetype.h>
// #define STB_VORBIS_HEADER_ONLY
// #include <stb_vorbis.h>

#include "stb_truetype.c.inc"
// #include "darnit_api.h"
#include "darnit_filesystem.h"
#include "darnit_bitwise.h"
#include "darnit_tilesheet.h"
#include "darnit_render.h"
#include "darnit_input.h"
#include "darnit_error.h"
#include "darnit_text.h"
#include "darnit_menutk.h"
#include "darnit_audio.h"
#include "darnit_imgload.h"
#include "darnit_sprite.h"
#include "darnit_bbox.h"
#include "darnit_render_tilemap.h"
#include "darnit_tilemap.h"
#include "darnit_dynlib.h"
#include "darnit_socket.h"
#include "darnit_particle.h"
#include "darnit_mt_sprite.h"
#include "darnit_audio_load.h"
#include "darnit_utf8.h"
#include "darnit_util.h"
#include "darnit_stringtable.h"
#include "darnit_ldmz.h"
#include "darnit_isometric.h"
#include "darnit_collision.h"
#include "darnit_compression.h"
#include "darnit_tbbox.h"

int darnitTimeLastFrameTook();


typedef struct {
	unsigned int		platform;
	unsigned int		screen_w;
	unsigned int		screen_h;
	unsigned int		fullscreen;
} DARNIT_PLATFORM;


typedef struct {
	TILE_CACHE		*tc;
	unsigned int		tiles;
	TILESHEET		*ts;
} DARNIT_RENDER_BUFFER;


typedef struct {
	LINE_CACHE		*lc;
	unsigned int		lines;
	unsigned int		line_w;
} DARNIT_RENDER_LINE_BUFFER;


typedef struct {
	RECT_CACHE		*rc;
	unsigned int		rects;
} DARNIT_RENDER_RECT_BUFFER;


typedef struct {
	POINT_CACHE		*pc;
	unsigned int		points;
	unsigned int		point_w;
} DARNIT_RENDER_POINT_BUFFER;


typedef struct {
	int			frames_counting;
	int			frames_last;
	int			time;
	int			time_at_flip;
	int			time_at_last_frame;
} DARNIT_FPS;


typedef struct {
	VIDEO			video;
	INPUT_STRUCT		input;
	AUDIO			audio;
	FILESYSTEM		fs;
	DARNIT_FPS		fps;
	TILESHEET_REF_CONT	tsr;
	DARNIT_PLATFORM		platform;
	SOCKET_LIST		*connect_list;
	UTIL			util;
	RENDER_PARAMS		render_params;
} DARNIT;


/* It has finally happened. I made that annoying handle global. I know <.< */
extern DARNIT *darnit_main_struct_that_nothing_should_use;
#define	d	darnit_main_struct_that_nothing_should_use


#endif
