#ifndef SDLUNA_H
#define SDLUNA_H

#include <luaMagic.hpp>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer2.h>
#include <SDL_ttf.h>

/* Cancel SDL_main */
//#ifdef main
//#undef main
//#endif

/********************************************************************
**********************Template specializations***********************
********************************************************************/
#include "sdluna_Mix_Music.hpp"
#include "sdluna_SDL_bool.hpp"
#include "sdluna_SDL_Event.hpp"
#include "sdluna_SDL_Point.hpp"
#include "sdluna_SDL_Rect.hpp"
#include "sdluna_SDL_Renderer.hpp"
#include "sdluna_SDL_Texture.hpp"
#include "sdluna_SDL_Window.hpp"
#include "sdluna_TTF_Font.hpp"



/********************************************************************
****************************Bind Functions***************************
********************************************************************/
void SDLuna_VedioBind(lua_State* L);
void SDLuna_RenderBind(lua_State* L);
void SDLuna_EventBind(lua_State* L);
void SDLuna_ImageBind(lua_State* L);
void SDLuna_TimerBind(lua_State* L);
void SDLuna_KeyboardBind(lua_State* L);
void SDLuna_DrawBind(lua_State* L);
void SDLuna_MusicBind(lua_State* L);
void SDLuna_ErrorBind(lua_State* L);
void SDLuna_TtfBind(lua_State* L);


#endif