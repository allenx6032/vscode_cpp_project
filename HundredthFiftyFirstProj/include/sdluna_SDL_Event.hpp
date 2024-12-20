#ifndef SDLUNA_SDL_EVENT_H
#define SDLUNA_SDL_EVENT_H

#include <luaMagic.hpp>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer2.h>
#include <SDL_ttf.h>

static void SDLuna_PushKey(lua_State* L, SDL_Keycode key);
static void SDLuna_PushMouseButto(lua_State* L, Uint8 mouse);

/* Write SDL_Event* to Lua */
template<> 
inline void luaMagic_write(lua_State* L, SDL_Event* value)
{	
	/* nil */
	if(value == nullptr)
	{
		lua_pushnil(L);
		return;
	}
	
	/* Create a table to save event */
	lua_newtable(L);
	
	/* Store values */
	luaMagic_write(L, value->type);
	lua_setfield(L, -2, "type");
	
	switch(value->type)
	{
	case SDL_QUIT:
		luaMagic_write(L, value->quit.timestamp);
		lua_setfield(L, -2, "timestamp");
		break;
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		
		SDLuna_PushKey(L, value->key.keysym.sym);
		
		break;

	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		SDLuna_PushMouseButto(L, value->button.button);
		luaMagic_write(L, value->button.x);
		lua_setfield(L, -2, "x");
		luaMagic_write(L, value->button.y);
		lua_setfield(L, -2, "y");
		luaMagic_write(L, value->button.clicks);
		lua_setfield(L, -2, "clicks");
		break;
	}
	
	
}

/* Read SDL_Event* from Lua */
template<>
inline SDL_Event* luaMagic_read<SDL_Event*>(lua_State* L, int index)
{
	void* value = lua_touserdata(L, index);
	return static_cast<SDL_Event*>(value);
}

/* Push mouse value */
static void SDLuna_PushMouseButto(lua_State* L, Uint8 mouse)
{
	switch(mouse)
	{
	case SDL_BUTTON_LEFT:
		lua_pushstring(L, "left");
		break;
	case SDL_BUTTON_RIGHT:
		lua_pushstring(L, "right");
		break;
	case SDL_BUTTON_MIDDLE:
		lua_pushstring(L, "middle");
		break;
	default:
		lua_pushstring(L, "unknown");
		break;
	}

	lua_setfield(L, -2, "button");
}

/* Push key value */
static void SDLuna_PushKey(lua_State* L, SDL_Keycode key)
{
	lua_pushstring(L, "key");
	
	switch(key)
	{
	/* Default */
	default:
		lua_pushfstring (L, "Unsuppoeted Key : %d.", key);
		break;
	
		
	/********** Basic **********/
	case SDLK_ESCAPE:
		lua_pushstring(L, "esc");
		break;
	case SDLK_SPACE:
		lua_pushstring(L, "space");
		break;
	case SDLK_BACKSPACE:
		lua_pushstring(L, "backspace");
		break;
	case SDLK_TAB:
		lua_pushstring(L, "tab");
		break;
	case SDLK_RETURN:
		lua_pushstring(L, "enter");
		break;
	case SDLK_LALT:
	case SDLK_RALT:
		lua_pushstring(L, "alt");
		break;
	case SDLK_LCTRL:
	case SDLK_RCTRL:
		lua_pushstring(L, "ctrl");
		break;
	case SDLK_LSHIFT:
	case SDLK_RSHIFT:
		lua_pushstring(L, "shift");
		break;
		
	/********** Direction Key **********/
	case SDLK_UP:
		lua_pushstring(L, "up");
		break;
	case SDLK_DOWN:
		lua_pushstring(L, "down");
		break;
	case SDLK_LEFT:
		lua_pushstring(L, "left");
		break;
	case SDLK_RIGHT:
		lua_pushstring(L, "right");
		break;
		
	/********** Number Key **********/
	case SDLK_0:
		lua_pushstring(L, "0");
		break;
	case SDLK_1:
		lua_pushstring(L, "1");
		break;
	case SDLK_2:
		lua_pushstring(L, "2");
		break;
	case SDLK_3:
		lua_pushstring(L, "3");
		break;
	case SDLK_4:
		lua_pushstring(L, "4");
		break;
	case SDLK_5:
		lua_pushstring(L, "5");
		break;
	case SDLK_6:
		lua_pushstring(L, "6");
		break;
	case SDLK_7:
		lua_pushstring(L, "7");
		break;
	case SDLK_8:
		lua_pushstring(L, "8");
		break;
	case SDLK_9:
		lua_pushstring(L, "9");
		break;
		
	/********** Alphabet Key **********/
	case SDLK_a:
		lua_pushstring(L, "a");
		break;
	case SDLK_b:
		lua_pushstring(L, "b");
		break;
	case SDLK_c:
		lua_pushstring(L, "c");
		break;
	case SDLK_d:
		lua_pushstring(L, "d");
		break;
	case SDLK_e:
		lua_pushstring(L, "e");
		break;
	case SDLK_f:
		lua_pushstring(L, "f");
		break;
	case SDLK_g:
		lua_pushstring(L, "g");
		break;
	case SDLK_h:
		lua_pushstring(L, "h");
		break;
	case SDLK_i:
		lua_pushstring(L, "i");
		break;
	case SDLK_j:
		lua_pushstring(L, "j");
		break;
	case SDLK_k:
		lua_pushstring(L, "k");
		break;
	case SDLK_l:
		lua_pushstring(L, "l");
		break;
	case SDLK_m:
		lua_pushstring(L, "m");
		break;
	case SDLK_n:
		lua_pushstring(L, "n");
		break;
	case SDLK_o:
		lua_pushstring(L, "o");
		break;
	case SDLK_p:
		lua_pushstring(L, "p");
		break;
	case SDLK_q:
		lua_pushstring(L, "q");
		break;
	case SDLK_r:
		lua_pushstring(L, "r");
		break;
	case SDLK_s:
		lua_pushstring(L, "s");
		break;
	case SDLK_t:
		lua_pushstring(L, "t");
		break;
	case SDLK_u:
		lua_pushstring(L, "u");
		break;
	case SDLK_v:
		lua_pushstring(L, "v");
		break;
	case SDLK_w:
		lua_pushstring(L, "w");
		break;
	case SDLK_x:
		lua_pushstring(L, "x");
		break;
	case SDLK_y:
		lua_pushstring(L, "y");
		break;
	case SDLK_z:
		lua_pushstring(L, "z");
		break;
	
	/********** Fn **********/
	case SDLK_F1:
		lua_pushstring(L, "F1");
		break;
	case SDLK_F2:
		lua_pushstring(L, "F2");
		break;
	case SDLK_F3:
		lua_pushstring(L, "F3");
		break;
	case SDLK_F4:
		lua_pushstring(L, "F4");
		break;
	case SDLK_F5:
		lua_pushstring(L, "F5");
		break;
	case SDLK_F6:
		lua_pushstring(L, "F6");
		break;
	case SDLK_F7:
		lua_pushstring(L, "F7");
		break;
	case SDLK_F8:
		lua_pushstring(L, "F8");
		break;
	case SDLK_F9:
		lua_pushstring(L, "F9");
		break;
	case SDLK_F10:
		lua_pushstring(L, "F10");
		break;
	case SDLK_F11:
		lua_pushstring(L, "F11");
		break;
	case SDLK_F12:
		lua_pushstring(L, "F12");
		break;
		
	/********** Symbol Key **********/
	case SDLK_COMMA:
		lua_pushstring(L, ",");
		break;
	case SDLK_PERIOD:
		lua_pushstring(L, ".");
		break;
	case SDLK_SLASH:
		lua_pushstring(L, "/");
		break;
	case SDLK_SEMICOLON:
		lua_pushstring(L, ";");
		break;
	case SDLK_QUOTE:
		lua_pushstring(L, "\'");
		break;
	case SDLK_LEFTBRACKET:
		lua_pushstring(L, "[");
		break;
	case SDLK_RIGHTBRACKET:
		lua_pushstring(L, "]");
		break;
	case SDLK_BACKSLASH:
		lua_pushstring(L, "\\");
		break;
	case SDLK_MINUS:
		lua_pushstring(L, "-");
		break;
	case SDLK_EQUALS:
		lua_pushstring(L, "=");
		break;
	case SDLK_BACKQUOTE:
		lua_pushstring(L, "`");
		break;
	}
	
	lua_settable(L,-3);
}
#endif