#ifndef SDLUNA_WINDOW_H
#define SDLUNA_WINDOW_H

#include <luaMagic.hpp>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer2.h>
#include <SDL_ttf.h>

#define UDATA_SDL_WINDOW "Window"
inline int CreateWindowMetatable(lua_State* L);

/* Write SDL_Window* to Lua */
template<> 
inline void luaMagic_write(lua_State* L, SDL_Window* value)
{	
	/* Create GC meta-table only once */
	CreateWindowMetatable(L);

	if(value == nullptr)
	{
		lua_pushnil(L);
	}
	else
	{
		SDL_Window** ptr = static_cast<SDL_Window**>(lua_newuserdata(L, sizeof(value)));
		*ptr = value;
		
		/* Set __gc meta-method */
		lua_getfield(L, LUA_REGISTRYINDEX, UDATA_SDL_WINDOW);
		lua_setmetatable(L, -2);
	}
}

/* Read SDL_Window* from Lua */
template<>
inline SDL_Window* luaMagic_read<SDL_Window*>(lua_State* L, int index)
{
	return *static_cast<SDL_Window**>(luaL_checkudata(L, index, UDATA_SDL_WINDOW));
}


/* Create a meta-table */
inline int CreateWindowMetatable(lua_State* L)
{
	// create metatable with __name = UDATA_SDL_WINDOW
	if(luaL_newmetatable(L, UDATA_SDL_WINDOW))
	{
		// __gc meta-method to recycle SDL_Window
		lua_pushcfunction(L, [](lua_State* L)->int{
			SDL_Window* ptr = *static_cast<SDL_Window**>(lua_touserdata(L, 1));
			SDL_DestroyWindow(ptr);
			return 0;
		});
		lua_setfield(L, -2, "__gc");

		// __index meta-method to read property
		lua_pushcfunction(L, [](lua_State* L)->int{
			SDL_Window* window = luaMagic_read<SDL_Window*>(L, 1);
			const char* index = luaMagic_read<const char*>(L, 2);
			int w = -1;
			int h = -1;
			SDL_GetWindowSize(window, &w, &h);
			if(strcmp(index, "width") == 0)
			{
				luaMagic_write(L, w);
			}
			else if(strcmp(index, "height") == 0)
			{
				luaMagic_write(L, h);
			}
			else
			{
				lua_pushnil(L);
			}

			return 1;
		});
	}
	lua_setfield(L, -2, "__index");

	return 0;
}

#endif