#include "sdluna.hpp"

SDL_Texture* SDLuna_LoadImage(SDL_Renderer* renderer, const char* file);

/* Bind Functions */
void SDLuna_ImageBind(lua_State* L)
{
	/* Functions */
	luaMagic::bind(L, "LoadImage", SDLuna_LoadImage, true);
}

SDL_Texture* SDLuna_LoadImage(SDL_Renderer* renderer, const char* file)
{
	SDL_Surface* surface = IMG_Load(file);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer,surface);
	SDL_FreeSurface(surface);
	return texture;
}
