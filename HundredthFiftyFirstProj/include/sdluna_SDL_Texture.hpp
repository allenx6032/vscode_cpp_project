#ifndef SDLUNA_TEXTURE_H
#define SDLUNA_TEXTURE_H

#include <luaMagic.hpp>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer2.h>
#include <SDL_ttf.h>

#define UDATA_SDL_TEXTURE "Texture"
inline int CreateTextureMetatable(lua_State* L);

/* Write SDL_Texture* to Lua */
template<> 
inline void luaMagic_write(lua_State* L, SDL_Texture* value)
{	
	/* Create GC meta-table only once */
	CreateTextureMetatable(L);

	if(value == nullptr)
	{
		lua_pushnil(L);
	}
	else
	{
		SDL_Texture** ptr = static_cast<SDL_Texture**>(lua_newuserdata(L, sizeof(value)));
		*ptr = value;
		
		/* Set __gc meta-method */
		lua_getfield(L, LUA_REGISTRYINDEX, UDATA_SDL_TEXTURE);
		lua_setmetatable(L, -2);
	}
}

/* Read SDL_Texture* from Lua */
template<>
inline SDL_Texture* luaMagic_read<SDL_Texture*>(lua_State* L, int index)
{
	return *static_cast<SDL_Texture**>(luaL_checkudata(L, index, UDATA_SDL_TEXTURE));
}


/* Create a meta-table */
inline int CreateTextureMetatable(lua_State* L)
{
	// create metatable with __name = UDATA_SDL_TEXTURE
	if(luaL_newmetatable(L, UDATA_SDL_TEXTURE))
	{
		// __gc meta-method to recycle SDL_Texture
		lua_pushcfunction(L, [](lua_State* L)->int{
			SDL_Texture* ptr = *static_cast<SDL_Texture**>(lua_touserdata(L, 1));
			SDL_DestroyTexture(ptr);
			return 0;
		});
		lua_setfield(L, -2, "__gc");
	}

	// __index meta-method to read property
	lua_pushcfunction(L, [](lua_State* L)->int{
		SDL_Texture* texture = luaMagic_read<SDL_Texture*>(L, 1);
		const char* index = luaMagic_read<const char*>(L, 2);
		Uint32 format = SDL_PIXELFORMAT_UNKNOWN;
		int access = -1;
		int w = -1;
		int h = -1;
		SDL_QueryTexture(texture, &format, &access, &w, &h);
		if(strcmp(index, "format") == 0)
		{
			luaMagic_write(L, format);
		}
		else if(strcmp(index, "access") == 0)
		{
			luaMagic_write(L, access);
		}
		else if(strcmp(index, "width") == 0)
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
	lua_setfield(L, -2, "__index");

	return 0;
}

#endif