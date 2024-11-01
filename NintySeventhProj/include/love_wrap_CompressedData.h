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

#ifndef LOVE_IMAGE_WRAP_COMRESSED_DATA_H
#define LOVE_IMAGE_WRAP_COMRESSED_DATA_H

// LOVE
#include "love_runtime.h"
#include "love_CompressedData.h"

namespace love
{
namespace image
{

CompressedData *luax_checkcompresseddata(lua_State *L, int idx);
int w_CompressedData_getWidth(lua_State *L);
int w_CompressedData_getHeight(lua_State *L);
int w_CompressedData_getDimensions(lua_State *L);
int w_CompressedData_getMipmapCount(lua_State *L);
int w_CompressedData_getFormat(lua_State *L);
extern "C" int luaopen_compresseddata(lua_State *L);

} // image
} // love

#endif // LOVE_IMAGE_WRAP_COMRESSED_DATA_H
