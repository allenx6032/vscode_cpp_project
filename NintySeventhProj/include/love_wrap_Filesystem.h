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

#ifndef LOVE_FILESYSTEM_WRAP_FILESYSTEM_H
#define LOVE_FILESYSTEM_WRAP_FILESYSTEM_H

// LOVE
#include "love_runtime.h"
#include "love_FileData.h"

namespace love
{
namespace filesystem
{

/**
 * Gets FileData at the specified index. If the index contains a filepath or
 * a File object, the FileData will be created from that.
 * Note that this function retains the FileData object (possibly by creating it),
 * so a matching release() is required!
 * May trigger a Lua error.
 **/
FileData *luax_getfiledata(lua_State *L, int idx);

bool hack_setupWriteDirectory();
int w_init(lua_State *L);
int w_setFused(lua_State *L);
int w_isFused(lua_State *L);
int w_setIdentity(lua_State *L);
int w_getIdentity(lua_State *L);
int w_setSource(lua_State *L);
int w_getSource(lua_State *L);
int w_mount(lua_State *L);
int w_unmount(lua_State *L);
int w_newFile(lua_State *L);
int w_newFileData(lua_State *L);
int w_getWorkingDirectory(lua_State *L);
int w_getUserDirectory(lua_State *L);
int w_getAppdataDirectory(lua_State *L);
int w_getSaveDirectory(lua_State *L);
int w_getSourceBaseDirectory(lua_State *L);
int w_getRealDirectory(lua_State *L);
int w_exists(lua_State *L);
int w_isDirectory(lua_State *L);
int w_isFile(lua_State *L);
int w_createDirectory(lua_State *L);
int w_remove(lua_State *L);
int w_open(lua_State *L);
int w_close(lua_State *L);
int w_read(lua_State *L);
int w_write(lua_State *L);
int w_append(lua_State *L);
int w_getDirectoryItems(lua_State *L);
int w_lines(lua_State *L);
int w_load(lua_State *L);
int w_getLastModified(lua_State *L);
int w_getSize(lua_State *L);
int w_setSymlinksEnabled(lua_State *L);
int w_areSymlinksEnabled(lua_State *L);
int w_isSymlink(lua_State *L);
int loader(lua_State *L);
int extloader(lua_State *L);
extern "C" LOVE_EXPORT int luaopen_love_filesystem(lua_State *L);

} // filesystem
} // love

#endif // LOVE_FILESYSTEM_WRAP_FILESYSTEM_H
