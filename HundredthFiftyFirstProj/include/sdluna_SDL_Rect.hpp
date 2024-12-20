#ifndef SDLUNA_SDL_RECT_H
#define SDLUNA_SDL_RECT_H

#include <luaMagic.hpp>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer2.h>
#include <SDL_ttf.h>

/* Write SDL_Rect* to Lua */
template<> 
inline void luaMagic_write(lua_State* L, SDL_Rect* value)
{	
	
}

/* Read SDL_Rect* from Lua */
template<>
inline const SDL_Rect* luaMagic_read<const SDL_Rect*>(lua_State* L, int index)
{
	if(lua_isnoneornil(L, index))
	{
		return nullptr;
	}
	
	static SDL_Rect rect;
	lua_rawgeti(L, index, 1);
	rect.x = static_cast<int>(luaL_checkinteger(L,-1));
	lua_rawgeti(L, index, 2);
	rect.y = static_cast<int>(luaL_checkinteger(L,-1));
	lua_rawgeti(L, index, 3);
	rect.w = static_cast<int>(luaL_checkinteger(L,-1));
	lua_rawgeti(L, index, 4);
	rect.h = static_cast<int>(luaL_checkinteger(L,-1));
	return &rect;
}

#endif