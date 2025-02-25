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

#include "love_config.h"

#include "love_wrap_Keyboard.h"

#include "love_sdl_Keyboard.h"

namespace love
{
namespace keyboard
{

#define instance() (Module::getInstance<Keyboard>(Module::M_KEYBOARD))

int w_setKeyRepeat(lua_State *L)
{
	instance()->setKeyRepeat(luax_toboolean(L, 1));
	return 0;
}

int w_hasKeyRepeat(lua_State *L)
{
	luax_pushboolean(L, instance()->hasKeyRepeat());
	return 1;
}

int w_isDown(lua_State *L)
{
	Keyboard::Key k;
	unsigned int num = lua_gettop(L);
	Keyboard::Key *keylist = new Keyboard::Key[num+1];
	unsigned int counter = 0;

	for (unsigned int i = 0; i < num; i++)
	{
		if (Keyboard::getConstant(luaL_checkstring(L, i+1), k))
			keylist[counter++] = k;
	}
	keylist[counter] = Keyboard::KEY_MAX_ENUM;

	luax_pushboolean(L, instance()->isDown(keylist));
	delete[] keylist;
	return 1;
}

int w_getScancodeFromKey(lua_State *L)
{
	const char *keystr = luaL_checkstring(L, 1);
	Keyboard::Key key;
	if (!Keyboard::getConstant(keystr, key))
		return luaL_error(L, "Invalid key constant: %s", keystr);

	Keyboard::Scancode scancode = instance()->getScancodeFromKey(key);

	const char *scancodestr;
	if (!Keyboard::getConstant(scancode, scancodestr))
		return luaL_error(L, "Unknown scancode.");

	lua_pushstring(L, scancodestr);
	return 1;
}

int w_getKeyFromScancode(lua_State *L)
{
	const char *scancodestr = luaL_checkstring(L, 1);
	Keyboard::Scancode scancode;
	if (!Keyboard::getConstant(scancodestr, scancode))
		return luaL_error(L, "Invalid scancode: %s", scancode);

	Keyboard::Key key = instance()->getKeyFromScancode(scancode);

	const char *keystr;
	if (!Keyboard::getConstant(key, keystr))
		return luaL_error(L, "Unknown key constant");

	lua_pushstring(L, keystr);
	return 1;
}

int w_setTextInput(lua_State *L)
{
	instance()->setTextInput(luax_toboolean(L, 1));
	return 0;
}

int w_hasTextInput(lua_State *L)
{
	luax_pushboolean(L, instance()->hasTextInput());
	return 1;
}

// List of functions to wrap.
static const luaL_Reg functions[] =
{
	{ "setKeyRepeat", w_setKeyRepeat },
	{ "hasKeyRepeat", w_hasKeyRepeat },
	{ "setTextInput", w_setTextInput },
	{ "hasTextInput", w_hasTextInput },
	{ "isDown", w_isDown },
	{ "getScancodeFromKey", w_getScancodeFromKey },
	{ "getKeyFromScancode", w_getKeyFromScancode },
	{ 0, 0 }
};

extern "C" int luaopen_love_keyboard(lua_State *L)
{
	Keyboard *instance = instance();
	if (instance == nullptr)
	{
		luax_catchexcept(L, [&](){ instance = new love::keyboard::sdl::Keyboard(); });
	}
	else
		instance->retain();

	WrappedModule w;
	w.module = instance;
	w.name = "keyboard";
	w.flags = MODULE_T;
	w.functions = functions;
	w.types = 0;

	return luax_register_module(L, w);
}

} // keyboard
} // love
