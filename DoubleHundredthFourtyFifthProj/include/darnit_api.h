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
#define	__DARNIT_H__

#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
	DARNIT_PFORMAT_RGBA8 = 0,
	DARNIT_PFORMAT_RGBA4 = 1,
	DARNIT_PFORMAT_RGB5A1 = 2,
	DARNIT_PFORMAT_ALPHA = 3,
} DARNIT_PFORMAT;


#include <darnit_api_main.h>
#include <darnit_api_input.h>
#include <darnit_api_sound.h>
#include <darnit_api_render.h>
#include <darnit_api_sprite.h>
#include <darnit_api_mtsprite.h>
#include <darnit_api_text.h>
#include <darnit_api_menutk.h>
#include <darnit_api_tilemap.h>
#include <darnit_api_bbox.h>
#include <darnit_api_dynlib.h>
#include <darnit_api_socket.h>
#include <darnit_api_utf8.h>
#include <darnit_api_util.h>
#include <darnit_api_stringtable.h>
#include <darnit_api_filesystem.h>
#include <darnit_api_map.h>
#include <darnit_api_particle.h>
#include <darnit_api_collision.h>
#include <darnit_api_keysym.h>


#ifdef __cplusplus
}
#endif

#endif
