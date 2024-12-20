#ifndef SDLUNA_FONT_H
#define SDLUNA_FONT_H

#include <luaMagic.hpp>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer2.h>
#include <SDL_ttf.h>

#define UDATA_TTF_FONT "Font"
inline int CreateFontMetatable(lua_State* L);

/* Write TTF_Font* to Lua */
template<> 
inline void luaMagic_write(lua_State* L, TTF_Font* value)
{	
	/* Create GC meta-table only once */
	CreateFontMetatable(L);

	if(value == nullptr)
	{
		lua_pushnil(L);
	}
	else
	{
		TTF_Font** ptr = static_cast<TTF_Font**>(lua_newuserdata(L, sizeof(value)));
		*ptr = value;
		
		/* Set __gc meta-method */
		lua_getfield(L, LUA_REGISTRYINDEX, UDATA_TTF_FONT);
		lua_setmetatable(L, -2);
	}
}

/* Read TTF_Font* from Lua */
template<>
inline TTF_Font* luaMagic_read<TTF_Font*>(lua_State* L, int index)
{
	return *static_cast<TTF_Font**>(luaL_checkudata(L, index, UDATA_TTF_FONT));
}


/* Create a meta-table */
inline int CreateFontMetatable(lua_State* L)
{
	// create metatable with __name = UDATA_TTF_FONT
	if(luaL_newmetatable(L, UDATA_TTF_FONT))
	{
		// __gc meta-method to recycle TTF_Font
		lua_pushcfunction(L, [](lua_State* L)->int{
			TTF_Font* ptr = *static_cast<TTF_Font**>(lua_touserdata(L, 1));
			TTF_CloseFont(ptr);
			return 0;
		});
		lua_setfield(L, -2, "__gc");
	}
	return 0;
}

#endif