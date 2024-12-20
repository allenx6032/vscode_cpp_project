#ifndef SDLUNA_RENDERER_H
#define SDLUNA_RENDERER_H

#include <luaMagic.hpp>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer2.h>
#include <SDL_ttf.h>

#define UDATA_SDL_RENDERER "Renderer"
inline int CreateRendererMetatable(lua_State* L);

/* Write SDL_Renderer* to Lua */
template<> 
inline void luaMagic_write(lua_State* L, SDL_Renderer* value)
{	
	/* Create GC meta-table only once */
	CreateRendererMetatable(L);

	if(value == nullptr)
	{
		lua_pushnil(L);
	}
	else
	{
		SDL_Renderer** ptr = static_cast<SDL_Renderer**>(lua_newuserdata(L, sizeof(value)));
		*ptr = value;
		
		/* Set __gc meta-method */
		lua_getfield(L, LUA_REGISTRYINDEX, UDATA_SDL_RENDERER);
		lua_setmetatable(L, -2);
	}
}

/* Read SDL_Renderer* from Lua */
template<>
inline SDL_Renderer* luaMagic_read<SDL_Renderer*>(lua_State* L, int index)
{
	return *static_cast<SDL_Renderer**>(luaL_checkudata(L, index, UDATA_SDL_RENDERER));
}


/* Create a meta-table */
inline int CreateRendererMetatable(lua_State* L)
{
	// create metatable with __name = UDATA_SDL_RENDERER
	if(luaL_newmetatable(L, UDATA_SDL_RENDERER))
	{
		// __gc meta-method to recycle SDL_Renderer
		lua_pushcfunction(L, [](lua_State* L)->int{
			SDL_Renderer* ptr = *static_cast<SDL_Renderer**>(lua_touserdata(L, 1));
			SDL_DestroyRenderer(ptr);
			return 0;
		});
		lua_setfield(L, -2, "__gc");
	}
	return 0;
}

#endif