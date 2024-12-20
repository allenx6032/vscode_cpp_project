#include "sdluna.hpp"

#define SDLUNA_URL "https://github.com/hubenchang0515/SDLuna"
#define SDLUNA_GLOBAL_IDENTIFICATION "__SDLUNA_GLOBAL_IDENTIFICATION__"

bool SDLuna_Init(lua_State* L);
void SDLuna_Quit(void);

extern "C"
{
	int luaopen_libsdluna(lua_State* L)
	{
		/* Avoid to create object repeatedly */
		static bool firsttime = true;
		
		if(!firsttime)
		{
			/* SDLuna had been initialized before */
			lua_getfield(L, LUA_REGISTRYINDEX, SDLUNA_GLOBAL_IDENTIFICATION);
		}
		else if(SDLuna_Init(L))
		{
			/* Initialize SDLuna first time */
			firsttime = false;
			
			/* Bind */
			lua_newtable(L);
			SDLuna_VedioBind(L);
			SDLuna_RenderBind(L);
			SDLuna_EventBind(L);
			SDLuna_ImageBind(L);
			SDLuna_TimerBind(L);
			SDLuna_KeyboardBind(L);
			SDLuna_DrawBind(L);
			SDLuna_MusicBind(L);
			SDLuna_ErrorBind(L);
			SDLuna_TtfBind(L);
			
			// /* Set __gc meta-method*/
			lua_newtable(L);
			lua_pushcfunction(L, [](lua_State* L)->int{SDLuna_Quit(); return 0;});
			lua_setfield(L, -2, "__gc");
			lua_pushcfunction(L, [](lua_State* L)->int{lua_pushstring(L, SDLUNA_URL); return 1;});
			lua_setfield(L, -2, "__tostring");
			lua_setmetatable(L, -2);
			
			/* save */
			lua_setfield(L, LUA_REGISTRYINDEX, SDLUNA_GLOBAL_IDENTIFICATION);
			lua_getfield(L, LUA_REGISTRYINDEX, SDLUNA_GLOBAL_IDENTIFICATION);
		}
		else
		{
			SDLuna_Quit();
		}
		
		return 1;
	}
}



bool SDLuna_Init(lua_State* L)
{
	if(0 != SDL_Init(SDL_INIT_EVERYTHING))
	{
		#ifdef SDLUNA_DEBUG
			SDL_Log("SDL_Init failed.");
		#endif
		lua_pushstring(L, SDL_GetError());
		return false;
	}
	else if(0 == IMG_Init(IMG_INIT_JPG|IMG_INIT_PNG|IMG_INIT_TIF))
	{
		#ifdef SDLUNA_DEBUG
			SDL_Log("IMG_Init failed.");
		#endif
		lua_pushstring(L, IMG_GetError());
		return false;
	}
	else if(0 == Mix_Init(MIX_INIT_FLAC|MIX_INIT_MOD|MIX_INIT_MP3|MIX_INIT_OGG))
	{
		#ifdef SDLUNA_DEBUG
			SDL_Log("Mix_Init failed.");
		#endif
		lua_pushstring(L, Mix_GetError());
		return false;
	}
	else if(0 != TTF_Init())
	{
		#ifdef SDLUNA_DEBUG
			SDL_Log("TTF_Init failed.");
		#endif
		lua_pushstring(L, TTF_GetError());
		return false;
	}	
	else if(0 != Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048))
	{
		#ifdef SDLUNA_DEBUG
			SDL_Log("Mix_OpenAudio failed.");
		#endif
		lua_pushstring(L, Mix_GetError());
		return false;
	}
	
	return true;
}


void SDLuna_Quit(void)
{
	Mix_CloseAudio();
	SDL_Quit();
	IMG_Quit();
	Mix_Quit();
	TTF_Quit();
}