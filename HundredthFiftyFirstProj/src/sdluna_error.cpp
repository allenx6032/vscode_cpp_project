#include "sdluna.hpp"

const char* SDLuna_GetError();

void SDLuna_ErrorBind(lua_State* L)
{
	/* Functions */
	luaMagic::bind(L, "GetError", SDL_GetError, true);
	luaMagic::bind(L, "ClearError", SDL_ClearError, true);
}