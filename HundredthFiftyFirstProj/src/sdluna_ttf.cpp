#include "sdluna.hpp"

SDL_Texture* SDLuna_RenderText(SDL_Renderer* renderer, TTF_Font *font, const char* text);
SDL_Texture* SDLuna_RenderUTF8(SDL_Renderer* renderer, TTF_Font *font, const char* text);
SDL_Texture* SDLuna_RenderUnicode(SDL_Renderer* renderer, TTF_Font *font, const Uint16* text);

void SDLuna_TtfBind(lua_State* L)
{
	luaMagic::bind(L, "OpenFont", TTF_OpenFont, true);
	luaMagic::bind(L, "RenderText", SDLuna_RenderText, true);
	luaMagic::bind(L, "RenderUTF8", SDLuna_RenderUTF8, true);
	luaMagic::bind(L, "RenderUnicode", SDLuna_RenderUnicode, true);
	luaMagic::bind(L, "TtfError", TTF_GetError, true);
}

SDL_Texture* SDLuna_RenderText(SDL_Renderer* renderer, TTF_Font *font, const char* text)
{
	static SDL_Color color;
	SDL_GetRenderDrawColor(renderer, &(color.r), &(color.g), &(color.b), &(color.a));
	SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer,surface);
	SDL_FreeSurface(surface);

	return texture;
}


SDL_Texture* SDLuna_RenderUTF8(SDL_Renderer* renderer, TTF_Font *font, const char* text)
{
	static SDL_Color color;
	SDL_GetRenderDrawColor(renderer, &(color.r), &(color.g), &(color.b), &(color.a));
	SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer,surface);
	SDL_FreeSurface(surface);

	return texture;
}

SDL_Texture* SDLuna_RenderUnicode(SDL_Renderer* renderer, TTF_Font *font, const Uint16* text)
{
	static SDL_Color color;
	SDL_GetRenderDrawColor(renderer, &(color.r), &(color.g), &(color.b), &(color.a));
	SDL_Surface* surface = TTF_RenderUNICODE_Blended(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer,surface);
	SDL_FreeSurface(surface);
	
	return texture;
}