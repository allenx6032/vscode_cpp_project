#include "sdluna.hpp"

int SDLuna_PlayMusic(Mix_Music *music, bool loop);
int SDLuna_FadeInMusic(Mix_Music *music,int ms, bool loop);

/* Bind Functions */
void SDLuna_MusicBind(lua_State* L)
{
	/* Functions */
	luaMagic::bind(L, "LoadMusic", Mix_LoadMUS, true);
	luaMagic::bind(L, "PlayMusic", SDLuna_PlayMusic, true);
	luaMagic::bind(L, "PauseMusic", Mix_PauseMusic, true);
	luaMagic::bind(L, "ResumeMusic", Mix_ResumeMusic, true);
	luaMagic::bind(L, "RewindMusic", Mix_RewindMusic, true);
	luaMagic::bind(L, "FadeInMusic", SDLuna_FadeInMusic, true);
	luaMagic::bind(L, "FadeOutMusic", Mix_FadeOutMusic, true);
}

int SDLuna_PlayMusic(Mix_Music *music, bool loop)
{
	if(loop)
	{
		return Mix_PlayMusic(music, -1);
	}
	else
	{
		return Mix_PlayMusic(music, 1);
	}
}

int SDLuna_FadeInMusic(Mix_Music *music,int ms, bool loop)
{
	if(loop)
	{
		return Mix_FadeInMusic(music, -1, ms);
	}
	else
	{
		return Mix_FadeInMusic(music, 1, ms);
	}
}