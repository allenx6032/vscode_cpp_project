#include "sdluna.hpp"

SDL_Event* SDLuna_PollEvent(void);

/* Bind Functions */
void SDLuna_EventBind(lua_State* L)
{
	luaMagic::bind(L, "PollEvent", SDLuna_PollEvent, true);
	
	/* Event */
	luaMagic::setValue(L, "EVENT_QUIT", (int)SDL_QUIT, true);
	luaMagic::setValue(L, "EVENT_KEYDOWN", (int)SDL_KEYDOWN, true);
	luaMagic::setValue(L, "EVENT_KEYUP", (int)SDL_KEYUP, true);
	luaMagic::setValue(L, "EVENT_MOUSEMOTION", (int)SDL_MOUSEMOTION, true);
	luaMagic::setValue(L, "EVENT_MOUSEBUTTONDOWN", (int)SDL_MOUSEBUTTONDOWN, true);
	luaMagic::setValue(L, "EVENT_MOUSEBUTTONUP", (int)SDL_MOUSEBUTTONUP, true);
	luaMagic::setValue(L, "EVENT_MOUSEWHEEL", (int)SDL_MOUSEWHEEL, true);
	
	/* Key */
}


SDL_Event* SDLuna_PollEvent(void)
{
	static SDL_Event event;
	if(0 == SDL_PollEvent(&event))
		return nullptr;
	else
		return &event;
}