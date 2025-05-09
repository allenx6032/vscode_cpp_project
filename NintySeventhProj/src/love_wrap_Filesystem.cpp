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

// LOVE
#include "love_wrap_Filesystem.h"
#include "love_wrap_File.h"
#include "love_wrap_FileData.h"

#include "love_physfs_Filesystem.h"

// SDL
#include <SDL_loadso.h>

namespace love
{
namespace filesystem
{
	
#define instance() (Module::getInstance<physfs::Filesystem>(Module::M_FILESYSTEM))

bool hack_setupWriteDirectory()
{
	if (instance() != 0)
		return instance()->setupWriteDirectory();
	return false;
}

int w_init(lua_State *L)
{
	const char *arg0 = luaL_checkstring(L, 1);
	luax_catchexcept(L, [&](){ instance()->init(arg0); });
	return 0;
}

int w_setFused(lua_State *L)
{
	// no error checking needed, everything, even nothing
	// can be converted to a boolean
	instance()->setFused(luax_toboolean(L, 1));
	return 0;
}

int w_isFused(lua_State *L)
{
	luax_pushboolean(L, instance()->isFused());
	return 1;
}

int w_setIdentity(lua_State *L)
{
	const char *arg = luaL_checkstring(L, 1);
	bool append = luax_optboolean(L, 2, false);

	if (!instance()->setIdentity(arg, append))
		return luaL_error(L, "Could not set write directory.");

	return 0;
}

int w_getIdentity(lua_State *L)
{
	lua_pushstring(L, instance()->getIdentity());
	return 1;
}

int w_setSource(lua_State *L)
{
	const char *arg = luaL_checkstring(L, 1);

	if (!instance()->setSource(arg))
		return luaL_error(L, "Could not set source.");

	return 0;
}

int w_getSource(lua_State *L)
{
	lua_pushstring(L, instance()->getSource());
	return 1;
}

int w_mount(lua_State *L)
{
	const char *archive = luaL_checkstring(L, 1);
	const char *mountpoint = luaL_checkstring(L, 2);
	bool append = luax_optboolean(L, 3, false);

	luax_pushboolean(L, instance()->mount(archive, mountpoint, append));
	return 1;
}

int w_unmount(lua_State *L)
{
	const char *archive = luaL_checkstring(L, 1);

	luax_pushboolean(L, instance()->unmount(archive));
	return 1;
}

int w_newFile(lua_State *L)
{
	const char *filename = luaL_checkstring(L, 1);

	const char *str = 0;
	File::Mode mode = File::CLOSED;

	if (lua_isstring(L, 2))
	{
		str = luaL_checkstring(L, 2);
		if (!File::getConstant(str, mode))
			return luaL_error(L, "Incorrect file open mode: %s", str);
	}

	File *t = instance()->newFile(filename);

	if (mode != File::CLOSED)
	{
		try
		{
			if (!t->open(mode))
				throw love::Exception("Could not open file.");
		}
		catch (love::Exception &e)
		{
			t->release();
			return luax_ioError(L, "%s", e.what());
		}
	}

	luax_pushtype(L, "File", FILESYSTEM_FILE_T, t);
	t->release();
	return 1;
}

FileData *luax_getfiledata(lua_State *L, int idx)
{
	FileData *data = nullptr;
	File *file = nullptr;

	if (lua_isstring(L, idx))
	{
		const char *filename = luaL_checkstring(L, idx);
		file = instance()->newFile(filename);
	}
	else if (luax_istype(L, idx, FILESYSTEM_FILE_T))
	{
		file = luax_checkfile(L, idx);
		file->retain();
	}
	else if (luax_istype(L, idx, FILESYSTEM_FILE_DATA_T))
	{
		data = luax_checkfiledata(L, idx);
		data->retain();
	}

	if (!data && !file)
	{
		luaL_argerror(L, idx, "filename, File, or FileData expected");
		return nullptr; // Never reached.
	}

	if (file)
	{
		luax_catchexcept(L,
			[&]() { data = file->read(); },
			[&]() { file->release(); }
		);
	}

	return data;
}

int w_newFileData(lua_State *L)
{
	// Single argument: treat as filepath or File.
	if (lua_gettop(L) == 1)
	{
		// We don't use luax_getfiledata because we want to use an ioError.
		if (lua_isstring(L, 1))
			luax_convobj(L, 1, "filesystem", "newFile");

		// Get FileData from the File.
		if (luax_istype(L, 1, FILESYSTEM_FILE_T))
		{
			File *file = luax_checkfile(L, 1);

			FileData *data = 0;
			try
			{
				data = file->read();
			}
			catch (love::Exception &e)
			{
				return luax_ioError(L, "%s", e.what());
			}
			luax_pushtype(L, "FileData", FILESYSTEM_FILE_DATA_T, data);
			data->release();
			return 1;
		}
		else
			return luaL_argerror(L, 1, "filename or File expected");
	}

	size_t length = 0;
	const char *str = luaL_checklstring(L, 1, &length);
	const char *filename = luaL_checkstring(L, 2);
	const char *decstr = lua_isstring(L, 3) ? lua_tostring(L, 3) : 0;

	FileData::Decoder decoder = FileData::FILE;

	if (decstr && !FileData::getConstant(decstr, decoder))
		return luaL_error(L, "Invalid FileData decoder: %s", decstr);

	FileData *t = 0;

	switch (decoder)
	{
	case FileData::FILE:
		t = instance()->newFileData((void *)str, (int)length, filename);
		break;
	case FileData::BASE64:
		t = instance()->newFileData(str, filename);
		break;
	default:
		return luaL_error(L, "Invalid FileData decoder: %s", decstr);
	}

	luax_pushtype(L, "FileData", FILESYSTEM_FILE_DATA_T, t);
	t->release();
	return 1;
}

int w_getWorkingDirectory(lua_State *L)
{
	lua_pushstring(L, instance()->getWorkingDirectory());
	return 1;
}

int w_getUserDirectory(lua_State *L)
{
	luax_pushstring(L, instance()->getUserDirectory());
	return 1;
}

int w_getAppdataDirectory(lua_State *L)
{
	luax_pushstring(L, instance()->getAppdataDirectory());
	return 1;
}

int w_getSaveDirectory(lua_State *L)
{
	lua_pushstring(L, instance()->getSaveDirectory());
	return 1;
}

int w_getSourceBaseDirectory(lua_State *L)
{
	luax_pushstring(L, instance()->getSourceBaseDirectory());
	return 1;
}

int w_getRealDirectory(lua_State *L)
{
	const char *filename = luaL_checkstring(L, 1);
	std::string dir;

	try
	{
		dir = instance()->getRealDirectory(filename);
	}
	catch (love::Exception &e)
	{
		return luax_ioError(L, "%s", e.what());
	}

	lua_pushstring(L, dir.c_str());
	return 1;
}

int w_exists(lua_State *L)
{
	const char *arg = luaL_checkstring(L, 1);
	luax_pushboolean(L, instance()->exists(arg));
	return 1;
}

int w_isDirectory(lua_State *L)
{
	const char *arg = luaL_checkstring(L, 1);
	luax_pushboolean(L, instance()->isDirectory(arg));
	return 1;
}

int w_isFile(lua_State *L)
{
	const char *arg = luaL_checkstring(L, 1);
	luax_pushboolean(L, instance()->isFile(arg));
	return 1;
}

int w_createDirectory(lua_State *L)
{
	const char *arg = luaL_checkstring(L, 1);
	luax_pushboolean(L, instance()->createDirectory(arg));
	return 1;
}

int w_remove(lua_State *L)
{
	const char *arg = luaL_checkstring(L, 1);
	luax_pushboolean(L, instance()->remove(arg));
	return 1;
}

int w_read(lua_State *L)
{
	const char *filename = luaL_checkstring(L, 1);
	int64 len = (int64) luaL_optinteger(L, 2, File::ALL);

	Data *data = 0;
	try
	{
		data = instance()->read(filename, len);
	}
	catch (love::Exception &e)
	{
		return luax_ioError(L, "%s", e.what());
	}

	if (data == 0)
		return luax_ioError(L, "File could not be read.");

	// Push the string.
	lua_pushlstring(L, (const char *) data->getData(), data->getSize());

	// Push the size.
	lua_pushinteger(L, data->getSize());

	// Lua has a copy now, so we can free it.
	data->release();

	return 2;
}

static int w_write_or_append(lua_State *L, File::Mode mode)
{
	const char *filename = luaL_checkstring(L, 1);

	const char *input = 0;
	size_t len = 0;

	if (luax_istype(L, 2, DATA_T))
	{
		love::Data *data = luax_totype<love::Data>(L, 2, "Data", DATA_T);
		input = (const char *) data->getData();
		len = data->getSize();
	}
	else if (lua_isstring(L, 2))
		input = lua_tolstring(L, 2, &len);
	else
		return luaL_argerror(L, 2, "string or Data expected");

	// Get how much we should write. Length of string default.
	len = luaL_optinteger(L, 3, len);

	try
	{
		if (mode == File::APPEND)
			instance()->append(filename, (const void *) input, len);
		else
			instance()->write(filename, (const void *) input, len);
	}
	catch (love::Exception &e)
	{
		return luax_ioError(L, "%s", e.what());
	}

	luax_pushboolean(L, true);
	return 1;
}

int w_write(lua_State *L)
{
	return w_write_or_append(L, File::WRITE);
}

int w_append(lua_State *L)
{
	return w_write_or_append(L, File::APPEND);
}

int w_getDirectoryItems(lua_State *L)
{
	return instance()->getDirectoryItems(L);
}

int w_lines(lua_State *L)
{
	File *file;

	if (lua_isstring(L, 1))
	{
		file = instance()->newFile(lua_tostring(L, 1));
		bool success = false;

		luax_catchexcept(L, [&](){ success = file->open(File::READ); });

		if (!success)
		{
			file->release();
			return luaL_error(L, "Could not open file.");
		}

		luax_pushtype(L, "File", FILESYSTEM_FILE_T, file);
		file->release();
	}
	else
		return luaL_argerror(L, 1, "expected filename.");

	lua_pushcclosure(L, physfs::Filesystem::lines_i, 1);
	return 1;
}

int w_load(lua_State *L)
{
	std::string filename = std::string(luaL_checkstring(L, 1));

	Data *data = 0;
	try
	{
		data = instance()->read(filename.c_str());
	}
	catch (love::Exception &e)
	{
		return luax_ioError(L, "%s", e.what());
	}

	int status = luaL_loadbuffer(L, (const char *)data->getData(), data->getSize(), ("@" + filename).c_str());

	data->release();

	// Load the chunk, but don't run it.
	switch (status)
	{
	case LUA_ERRMEM:
		return luaL_error(L, "Memory allocation error: %s\n", lua_tostring(L, -1));
	case LUA_ERRSYNTAX:
		return luaL_error(L, "Syntax error: %s\n", lua_tostring(L, -1));
	default: // success
		return 1;
	}
}

int w_getLastModified(lua_State *L)
{
	const char *filename = luaL_checkstring(L, 1);

	int64 time = 0;
	try
	{
		time = instance()->getLastModified(filename);
	}
	catch (love::Exception &e)
	{
		return luax_ioError(L, "%s", e.what());
	}

	lua_pushnumber(L, static_cast<lua_Number>(time));
	return 1;
}

int w_getSize(lua_State *L)
{
	const char *filename = luaL_checkstring(L, 1);

	int64 size = -1;
	try
	{
		size = instance()->getSize(filename);
	}
	catch (love::Exception &e)
	{
		return luax_ioError(L, "%s", e.what());
	}

	// Error on failure or if size does not fit into a double precision floating-point number.
	if (size == -1)
		return luax_ioError(L, "Could not determine file size.");
	else if (size >= 0x20000000000000LL)
		return luax_ioError(L, "Size too large to fit into a Lua number!");

	lua_pushnumber(L, (lua_Number) size);
	return 1;
}

int w_setSymlinksEnabled(lua_State *L)
{
	instance()->setSymlinksEnabled(luax_toboolean(L, 1));
	return 0;
}

int w_areSymlinksEnabled(lua_State *L)
{
	luax_pushboolean(L, instance()->areSymlinksEnabled());
	return 1;
}

int w_isSymlink(lua_State *L)
{
	const char *filename = luaL_checkstring(L, 1);
	luax_pushboolean(L, instance()->isSymlink(filename));
	return 1;
}

int loader(lua_State *L)
{
	const char *filename = lua_tostring(L, -1);

	std::string tmp(filename);
	tmp += ".lua";

	int size = tmp.size();

	for (int i=0; i<size-4; i++)
	{
		if (tmp[i] == '.')
		{
			tmp[i] = '/';
		}
	}

	// Check whether file exists.
	if (instance()->exists(tmp.c_str()))
	{
		lua_pop(L, 1);
		lua_pushstring(L, tmp.c_str());
		// Ok, load it.
		return w_load(L);
	}

	tmp = filename;
	size = tmp.size();
	for (int i=0; i<size; i++)
	{
		if (tmp[i] == '.')
		{
			tmp[i] = '/';
		}
	}

	if (instance()->isDirectory(tmp.c_str()))
	{
		tmp += "/init.lua";
		if (instance()->exists(tmp.c_str()))
		{
			lua_pop(L, 1);
			lua_pushstring(L, tmp.c_str());
			// Ok, load it.
			return w_load(L);
		}
	}

	std::string errstr = "\n\tno file '%s' in LOVE game directories.";
	errstr += errstr;

	lua_pushfstring(L, errstr.c_str(), (tmp + ".lua").c_str(), (tmp + "/init.lua").c_str());
	return 1;
}

inline const char *library_extension()
{
#ifdef LOVE_WINDOWS
	return ".dll";
#else
	return ".so";
#endif
}

int extloader(lua_State *L)
{
	const char *filename = lua_tostring(L, -1);
	std::string tokenized_name(filename);
	std::string tokenized_function(filename);

	for (unsigned int i = 0; i < tokenized_name.size(); i++)
	{
		if (tokenized_name[i] == '.')
		{
			tokenized_name[i] = '/';
			tokenized_function[i] = '_';
		}
	}

	tokenized_name += library_extension();

	void *handle = nullptr;

	// If the game is fused, try looking for the DLL in the game's read paths.
	if (instance()->isFused())
	{
		try
		{
			std::string dir = instance()->getRealDirectory(tokenized_name.c_str());

			// We don't want to look in the game's source, because it can be a
			// zip sometimes and a folder other times.
			if (dir.find(instance()->getSource()) == std::string::npos)
				handle = SDL_LoadObject((dir + LOVE_PATH_SEPARATOR + tokenized_name).c_str());
		}
		catch (love::Exception &)
		{
			// Nothing...
		}
	}

	if (!handle)
	{
		std::string path = std::string(instance()->getAppdataDirectory()) + LOVE_PATH_SEPARATOR LOVE_APPDATA_FOLDER LOVE_PATH_SEPARATOR + tokenized_name;
		handle = SDL_LoadObject(path.c_str());
	}

	if (!handle)
	{
		lua_pushfstring(L, "\n\tno file '%s' in LOVE paths.", tokenized_name.c_str());
		return 1;
	}

	void *func = SDL_LoadFunction(handle, ("loveopen_" + tokenized_function).c_str());
	if (!func)
		func = SDL_LoadFunction(handle, ("luaopen_" + tokenized_function).c_str());

	if (!func)
	{
		SDL_UnloadObject(handle);
		lua_pushfstring(L, "\n\tC library '%s' is incompatible.", tokenized_name.c_str());
		return 1;
	}

	lua_pushcfunction(L, (lua_CFunction) func);
	return 1;
}

// List of functions to wrap.
static const luaL_Reg functions[] =
{
	{ "init", w_init },
	{ "setFused", w_setFused },
	{ "isFused", w_isFused },
	{ "setIdentity", w_setIdentity },
	{ "getIdentity", w_getIdentity },
	{ "setSource", w_setSource },
	{ "getSource", w_getSource },
	{ "mount", w_mount },
	{ "unmount", w_unmount },
	{ "newFile", w_newFile },
	{ "getWorkingDirectory", w_getWorkingDirectory },
	{ "getUserDirectory", w_getUserDirectory },
	{ "getAppdataDirectory", w_getAppdataDirectory },
	{ "getSaveDirectory", w_getSaveDirectory },
	{ "getSourceBaseDirectory", w_getSourceBaseDirectory },
	{ "getRealDirectory", w_getRealDirectory },
	{ "exists", w_exists },
	{ "isDirectory", w_isDirectory },
	{ "isFile", w_isFile },
	{ "createDirectory", w_createDirectory },
	{ "remove", w_remove },
	{ "read", w_read },
	{ "write", w_write },
	{ "append", w_append },
	{ "getDirectoryItems", w_getDirectoryItems },
	{ "lines", w_lines },
	{ "load", w_load },
	{ "getLastModified", w_getLastModified },
	{ "getSize", w_getSize },
	{ "setSymlinksEnabled", w_setSymlinksEnabled },
	{ "areSymlinksEnabled", w_areSymlinksEnabled },
	{ "isSymlink", w_isSymlink },
	{ "newFileData", w_newFileData },
	{ 0, 0 }
};

static const lua_CFunction types[] =
{
	luaopen_file,
	luaopen_filedata,
	0
};

extern "C" int luaopen_love_filesystem(lua_State *L)
{
	physfs::Filesystem *instance = instance();
	if (instance == nullptr)
	{
		luax_catchexcept(L, [&](){ instance = new physfs::Filesystem(); });
	}
	else
		instance->retain();

	// The love loaders should be tried after package.preload.
	love::luax_register_searcher(L, loader, 2);
	love::luax_register_searcher(L, extloader, 3);

	WrappedModule w;
	w.module = instance;
	w.name = "filesystem";
	w.flags = MODULE_FILESYSTEM_T;
	w.functions = functions;
	w.types = types;

	return luax_register_module(L, w);
}

} // filesystem
} // love
