/**
 * Copyright (c) 2006-2015 LOVE Development Team
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 **/

#ifndef LOVE_GRAPHICS_OPENGL_WRAP_TEXTURE_H
#define LOVE_GRAPHICS_OPENGL_WRAP_TEXTURE_H

#include "love_opengl_Texture.h"
#include "love_runtime.h"

namespace love
{
namespace graphics
{
namespace opengl
{

Texture *luax_checktexture(lua_State *L, int idx);
int w_Texture_getWidth(lua_State *L);
int w_Texture_getHeight(lua_State *L);
int w_Texture_getDimensions(lua_State *L);
int w_Texture_setFilter(lua_State *L);
int w_Texture_getFilter(lua_State *L);
int w_Texture_setWrap(lua_State *L);
int w_Texture_getWrap(lua_State *L);

} // opengl
} // graphics
} // love

#endif // LOVE_GRAPHICS_OPENGL_WRAP_TEXTURE_H
