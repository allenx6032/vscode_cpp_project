#include "sdluna.hpp"
#include <map>

bool KeyPressed(const char* key);

void SDLuna_KeyboardBind(lua_State* L)
{
	/* Functions */
	luaMagic::bind(L, "KeyPressed", KeyPressed, true);
}

static std::map<std::string, SDL_Scancode> keyMap = {
	/* Basic Key */
	{"esc", SDL_SCANCODE_ESCAPE},
	{"space", SDL_SCANCODE_SPACE},
	{"backspace", SDL_SCANCODE_BACKSPACE},
	{"tab", SDL_SCANCODE_TAB},
	{"enter", SDL_SCANCODE_RETURN},
	{"left-alt", SDL_SCANCODE_LALT},
	{"right-alt", SDL_SCANCODE_RALT},
	{"left-ctrl", SDL_SCANCODE_LCTRL},
	{"right-ctrl", SDL_SCANCODE_RCTRL},
	{"left-shift", SDL_SCANCODE_LSHIFT},
	{"right-shift", SDL_SCANCODE_RSHIFT},
	
	/* Number Key */
	{"0", SDL_SCANCODE_0},
	{"1", SDL_SCANCODE_0},
	{"2", SDL_SCANCODE_0},
	{"3", SDL_SCANCODE_0},
	{"4", SDL_SCANCODE_0},
	{"5", SDL_SCANCODE_0},
	{"6", SDL_SCANCODE_0},
	{"7", SDL_SCANCODE_0},
	{"8", SDL_SCANCODE_0},
	{"9", SDL_SCANCODE_0},
	
	/* Alphabet Key */
	{"a", SDL_SCANCODE_A},
	{"b", SDL_SCANCODE_B},
	{"c", SDL_SCANCODE_C},
	{"d", SDL_SCANCODE_D},
	{"e", SDL_SCANCODE_E},
	{"f", SDL_SCANCODE_F},
	{"g", SDL_SCANCODE_G},
	{"h", SDL_SCANCODE_H},
	{"i", SDL_SCANCODE_I},
	{"j", SDL_SCANCODE_J},
	{"k", SDL_SCANCODE_K},
	{"l", SDL_SCANCODE_L},
	{"m", SDL_SCANCODE_M},
	{"n", SDL_SCANCODE_N},
	{"o", SDL_SCANCODE_O},
	{"p", SDL_SCANCODE_P},
	{"q", SDL_SCANCODE_Q},
	{"r", SDL_SCANCODE_R},
	{"s", SDL_SCANCODE_S},
	{"t", SDL_SCANCODE_T},
	{"u", SDL_SCANCODE_U},
	{"v", SDL_SCANCODE_V},
	{"w", SDL_SCANCODE_W},
	{"x", SDL_SCANCODE_X},
	{"y", SDL_SCANCODE_Y},
	{"z", SDL_SCANCODE_Z},
	
	/*  Direction Key */
	{"up", SDL_SCANCODE_UP},
	{"down", SDL_SCANCODE_DOWN},
	{"left", SDL_SCANCODE_LEFT},
	{"right", SDL_SCANCODE_RIGHT},
	
	/* Fn key */
	{"F1", SDL_SCANCODE_F1},
	{"F2", SDL_SCANCODE_F2},
	{"F3", SDL_SCANCODE_F3},
	{"F4", SDL_SCANCODE_F4},
	{"F5", SDL_SCANCODE_F5},
	{"F6", SDL_SCANCODE_F6},
	{"F7", SDL_SCANCODE_F7},
	{"F8", SDL_SCANCODE_F8},
	{"F9", SDL_SCANCODE_F9},
	{"F10", SDL_SCANCODE_F10},
	{"F11", SDL_SCANCODE_F11},
	{"F12", SDL_SCANCODE_F12},
	
	/* Symbol Key */
	{",", SDL_SCANCODE_COMMA},
	{".", SDL_SCANCODE_PERIOD},
	{"/", SDL_SCANCODE_SLASH},
	{";", SDL_SCANCODE_SEMICOLON},
	{"'", SDL_SCANCODE_APOSTROPHE},
	{"[", SDL_SCANCODE_LEFTBRACKET},
	{"]", SDL_SCANCODE_RIGHTBRACKET},
	{"\\", SDL_SCANCODE_BACKSLASH},
	{"-", SDL_SCANCODE_MINUS},
	{"=", SDL_SCANCODE_EQUALS},
	{"`", SDL_SCANCODE_GRAVE},
};

bool KeyPressed(const char* key)
{
	const uint8_t* keys = SDL_GetKeyboardState(nullptr);
	if(keys[keyMap.find(key)->second])
	{
		return true;
	}
	else
	{
		return false;
	}
}