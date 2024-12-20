#include "sdluna.hpp"

/* Bind Functions */
void SDLuna_VedioBind(lua_State* L)
{
	/* Functions */
	luaMagic::bind(L, "CreateWindow", SDL_CreateWindow, true);
	//luaMagic::bind(L, "DestroyWindow", SDL_DestroyWindow, true);
	//luaMagic::bind(L, "GetWindowWMInfo", SDL_GetWindowWMInfo, true);
	luaMagic::bind(L, "HideWindow", SDL_HideWindow, true);
	luaMagic::bind(L, "IsScreenSaverEnabled", SDL_IsScreenSaverEnabled, true);
	luaMagic::bind(L, "MaximizeWindow", SDL_MaximizeWindow, true);
	luaMagic::bind(L, "MinimizeWindow", SDL_MinimizeWindow, true);
	luaMagic::bind(L, "RaiseWindow", SDL_RaiseWindow, true);
	luaMagic::bind(L, "RestoreWindow", SDL_RestoreWindow, true);
	luaMagic::bind(L, "SetWindowBordered", SDL_SetWindowBordered, true);
	luaMagic::bind(L, "SetWindowBrightness", SDL_SetWindowBrightness, true);
	luaMagic::bind(L, "SetWindowData", SDL_SetWindowData, true);
	luaMagic::bind(L, "SetWindowDisplayMode", SDL_SetWindowDisplayMode, true);
	luaMagic::bind(L, "SetWindowFullscreen", SDL_SetWindowFullscreen, true);
	luaMagic::bind(L, "SetWindowGammaRamp", SDL_SetWindowGammaRamp, true);
	luaMagic::bind(L, "SetWindowGrab", SDL_SetWindowGrab, true);
	//luaMagic::bind(L, "SetWindowHitTest", SDL_SetWindowHitTest, true);
	luaMagic::bind(L, "SetWindowIcon", SDL_SetWindowIcon, true);
	// luaMagic::bind(L, "SetWindowInputFocus", SDL_SetWindowInputFocus, true);
	luaMagic::bind(L, "SetWindowMaximumSize", SDL_SetWindowMaximumSize, true);
	luaMagic::bind(L, "SetWindowMinimumSize", SDL_SetWindowMinimumSize, true);
	// luaMagic::bind(L, "SetWindowModalFor", SDL_SetWindowModalFor, true);
	// luaMagic::bind(L, "SetWindowOpacity", SDL_SetWindowOpacity, true);
	luaMagic::bind(L, "SetWindowPosition", SDL_SetWindowPosition, true);
	// luaMagic::bind(L, "SetWindowResizable", SDL_SetWindowResizable, true);
	luaMagic::bind(L, "SetWindowSize", SDL_SetWindowSize, true);
	luaMagic::bind(L, "SetWindowTitle", SDL_SetWindowTitle, true);
	//luaMagic::bind(L, "ShowMessageBox", SDL_ShowMessageBox, true);
	luaMagic::bind(L, "MessageBox", SDL_ShowSimpleMessageBox, true); /* rename */
	luaMagic::bind(L, "ShowWindow", SDL_ShowWindow, true);
	luaMagic::bind(L, "VideoInit", SDL_VideoInit, true);
	luaMagic::bind(L, "VideoQuit", SDL_VideoQuit, true);
	
	/* SDL_WindowPosition */
	luaMagic::setValue(L, "WINDOWPOS_CENTERED", (int)SDL_WINDOWPOS_CENTERED, true);
	luaMagic::setValue(L, "WINDOWPOS_UNDEFINED", (int)SDL_WINDOWPOS_UNDEFINED, true);
	
	/* SDL_WindowFlags */
	luaMagic::setValue(L, "WINDOW_BORDERLESS", (int)SDL_WINDOW_BORDERLESS, true);
	luaMagic::setValue(L, "WINDOW_SHOWN", (int)SDL_WINDOW_SHOWN, true);
	luaMagic::setValue(L, "WINDOW_HIDDEN", (int)SDL_WINDOW_HIDDEN, true);
	luaMagic::setValue(L, "WINDOW_RESIZABLE", (int)SDL_WINDOW_RESIZABLE, true);
	luaMagic::setValue(L, "WINDOW_MINIMIZED", (int)SDL_WINDOW_MINIMIZED, true);
	luaMagic::setValue(L, "WINDOW_MAXIMIZED", (int)SDL_WINDOW_MAXIMIZED, true);
	luaMagic::setValue(L, "WINDOW_FULLSCREEN", (int)SDL_WINDOW_FULLSCREEN, true);
	luaMagic::setValue(L, "WINDOW_FULLSCREEN_DESKTOP", (int)SDL_WINDOW_FULLSCREEN_DESKTOP, true);

	/* SDL_MessageBoxFlags */
	luaMagic::setValue(L, "MESSAGEBOX_ERROR", (int)SDL_MESSAGEBOX_ERROR, true);
	luaMagic::setValue(L, "MESSAGEBOX_WARNING", (int)SDL_MESSAGEBOX_WARNING, true);
	luaMagic::setValue(L, "MESSAGEBOX_INFORMATION", (int)SDL_MESSAGEBOX_INFORMATION, true);

	/* SDL_BlendMode */
	luaMagic::setValue(L, "BLENDMODE_NONE", (int)SDL_BLENDMODE_NONE, true);
	luaMagic::setValue(L, "BLENDMODE_BLEND", (int)SDL_BLENDMODE_BLEND, true);
	luaMagic::setValue(L, "BLENDMODE_ADD", (int)SDL_BLENDMODE_ADD, true);
	luaMagic::setValue(L, "BLENDMODE_MOD", (int)SDL_BLENDMODE_MOD, true);
}


int SDLuna_GetWindowSize(lua_State* L)
{
	SDL_Window* win = luaMagic_read<SDL_Window*>(L,1);
	int w,h;
	SDL_GetWindowSize(win, &w, &h);
	lua_newtable(L);
	luaMagic_write(L, w);
	lua_setfield(L, -2, "width");
	luaMagic_write(L, h);
	lua_setfield(L, -2, "height");

	return 1;
}