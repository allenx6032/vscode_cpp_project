#include "sdluna.hpp"

void DelayFps(unsigned int fps);
bool CheckFps(unsigned int fps);
int CreateCounter(lua_State* L);
int CheckCounter(lua_State* L);

/* Bind Functions */
void SDLuna_TimerBind(lua_State* L)
{
	/* Functions */
	luaMagic::bind(L, "Delay", SDL_Delay, true);
	luaMagic::bind(L, "DelayFps", DelayFps, true);
	luaMagic::bind(L, "GetTicks", SDL_GetTicks, true);
	luaMagic::bind(L, "CheckFps", CheckFps, true);
	
	lua_pushcfunction(L, CreateCounter);
	lua_setfield(L, -2, "CreateCounter");
}

void DelayFps(unsigned int fps)
{
	static uint32_t lastTick = SDL_GetTicks();
	uint32_t currentTick = SDL_GetTicks();
	SDL_Delay(1000/fps - (currentTick - lastTick));
	lastTick = SDL_GetTicks();
}

bool CheckFps(unsigned int fps)
{
	static uint32_t lastTick = SDL_GetTicks();
	uint32_t currentTick = SDL_GetTicks();
	if(currentTick - lastTick < 1000/fps)
	{
		return false;
	}
	else
	{
		lastTick = currentTick;
		return true;
	}
}

#define LAST_TICK "LAST_TICK"
int CreateCounter(lua_State* L)
{
	lua_newtable(L);
	lua_pushinteger(L, 0);
	lua_setfield(L, -2, LAST_TICK);
	lua_pushcfunction(L, CheckCounter);
	lua_setfield(L, -2, "time");
	
	return 1;
}

int CheckCounter(lua_State* L)
{
	lua_getfield(L, 1, LAST_TICK);
	lua_Integer delayms  = luaL_checkinteger(L, 2);
	lua_Integer lastTick = luaL_checkinteger(L, -1);
	
	if(lastTick != 0 && SDL_GetTicks() - lastTick < delayms)
	{
		lua_pushboolean(L, 0);
	}
	else
	{
		lua_pushinteger(L, SDL_GetTicks());
		lua_setfield(L, 1, LAST_TICK);
		lua_pushboolean(L, 1);
	}
	
	return 1;
}