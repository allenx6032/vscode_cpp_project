#include "sdluna.hpp"

static int SDLuna_SetRenderTarget(lua_State* L);

/* Bind Functions */
void SDLuna_RenderBind(lua_State* L)
{
	/* Functions */
	//luaMagic::bind(L, "ComposeCustomBlendMode", SDL_ComposeCustomBlendMode, true);
	luaMagic::bind(L, "CreateRenderer", SDL_CreateRenderer, true);
	luaMagic::bind(L, "CreateSoftwareRenderer", SDL_CreateSoftwareRenderer, true);
	luaMagic::bind(L, "CreateTexture", SDL_CreateTexture, true);
	luaMagic::bind(L, "CreateTextureFromSurface", SDL_CreateTextureFromSurface, true);
	luaMagic::bind(L, "CreateWindowAndRenderer", SDL_CreateWindowAndRenderer, true);
	luaMagic::bind(L, "DestroyRenderer", SDL_DestroyRenderer, true);
	//luaMagic::bind(L, "DestroyTexture", SDL_DestroyTexture, true);
	luaMagic::bind(L, "GL_BindTexture", SDL_GL_BindTexture, true);
	luaMagic::bind(L, "GL_UnbindTexture", SDL_GL_UnbindTexture, true);
	// luaMagic::bind(L, "GetNumRenderDrivers", SDL_GetNumRenderDrivers, true);
	// luaMagic::bind(L, "GetRenderDrawBlendMode", SDL_GetRenderDrawBlendMode, true);
	// luaMagic::bind(L, "GetRenderDrawColor", SDL_GetRenderDrawColor, true);
	// luaMagic::bind(L, "GetRenderDriverInfo", SDL_GetRenderDriverInfo, true);
	// luaMagic::bind(L, "GetRenderTarget", SDL_GetRenderTarget, true);
	// luaMagic::bind(L, "GetRenderer", SDL_GetRenderer, true);
	// luaMagic::bind(L, "GetRendererInfo", SDL_GetRendererInfo, true);
	// luaMagic::bind(L, "GetRendererOutputSize", SDL_GetRendererOutputSize, true);
	// luaMagic::bind(L, "GetTextureAlphaMod", SDL_GetTextureAlphaMod, true);
	// luaMagic::bind(L, "GetTextureBlendMode", SDL_GetTextureBlendMode, true);
	// luaMagic::bind(L, "GetTextureColorMod", SDL_GetTextureColorMod, true);
	luaMagic::bind(L, "LockTexture", SDL_LockTexture, true);
	luaMagic::bind(L, "QueryTexture", SDL_QueryTexture, true);
	luaMagic::bind(L, "RenderClear", SDL_RenderClear, true);
	luaMagic::bind(L, "RenderCopy", SDL_RenderCopy, true);
	//luaMagic::bind(L, "RenderCopyEx", SDL_RenderCopyEx, true);
	// luaMagic::bind(L, "RenderDrawLines", SDL_RenderDrawLines, true);
	// luaMagic::bind(L, "RenderDrawPoints", SDL_RenderDrawPoints, true);
	// luaMagic::bind(L, "RenderDrawRects", SDL_RenderDrawRects, true);
	luaMagic::bind(L, "RenderFillRects", SDL_RenderFillRects, true);
	luaMagic::bind(L, "RenderGetClipRect", SDL_RenderGetClipRect, true);
	luaMagic::bind(L, "RenderGetScale", SDL_RenderGetScale, true);
	luaMagic::bind(L, "RenderGetLogicalSize", SDL_RenderGetLogicalSize, true);
	luaMagic::bind(L, "RenderGetScale", SDL_RenderGetScale, true);
	luaMagic::bind(L, "RenderGetViewport", SDL_RenderGetViewport, true);
	luaMagic::bind(L, "RenderIsClipEnabled", SDL_RenderIsClipEnabled, true);
	luaMagic::bind(L, "RenderPresent", SDL_RenderPresent, true);
	luaMagic::bind(L, "RenderReadPixels", SDL_RenderReadPixels, true);
	luaMagic::bind(L, "RenderSetClipRect", SDL_RenderSetClipRect, true);
	// luaMagic::bind(L, "RenderSetIntegerScale", SDL_RenderSetIntegerScale, true);
	luaMagic::bind(L, "RenderSetLogicalSize", SDL_RenderSetLogicalSize, true);
	luaMagic::bind(L, "RenderSetScale", SDL_RenderSetScale, true);
	luaMagic::bind(L, "RenderSetViewport", SDL_RenderSetViewport, true);
	luaMagic::bind(L, "RenderTargetSupported", SDL_RenderTargetSupported, true);
	//luaMagic::bind(L, "SetRenderDrawBlendMode", SDL_SetRenderDrawBlendMode, true);
	luaMagic::bind(L, "SetRenderDrawColor", SDL_SetRenderDrawColor, true);
	//luaMagic::bind(L, "SetRenderTarget", SDL_SetRenderTarget, true);
	luaMagic::bind(L, "SetTextureAlphaMod", SDL_SetTextureAlphaMod, true);
	//luaMagic::bind(L, "SetTextureBlendMode", SDL_SetTextureBlendMode, true);
	luaMagic::bind(L, "SetTextureColorMod", SDL_SetTextureColorMod, true);
	luaMagic::bind(L, "UnlockTexture", SDL_UnlockTexture, true);
	luaMagic::bind(L, "UpdateTexture", SDL_UpdateTexture, true);
	luaMagic::bind(L, "UpdateYUVTexture", SDL_UpdateYUVTexture, true);
	
	/* SDL_RendererFlags */
	luaMagic::setValue(L, "RENDERER_SOFTWARE", (int)SDL_RENDERER_SOFTWARE, true);
	luaMagic::setValue(L, "RENDERER_ACCELERATED", (int)SDL_RENDERER_ACCELERATED, true);
	luaMagic::setValue(L, "RENDERER_PRESENTVSYNC", (int)SDL_RENDERER_PRESENTVSYNC, true);
	luaMagic::setValue(L, "RENDERER_TARGETTEXTURE", (int)SDL_RENDERER_TARGETTEXTURE, true);

	/* Pixel Format */
	luaMagic::setValue(L, "PIXELFORMAT_UNKNOWN", (uint32_t)SDL_PIXELFORMAT_UNKNOWN, true);
	luaMagic::setValue(L, "PIXELFORMAT_RGB888", (uint32_t)SDL_PIXELFORMAT_RGB888, true);
	luaMagic::setValue(L, "PIXELFORMAT_RGBA8888", (uint32_t)SDL_PIXELFORMAT_RGBA8888, true);
	
	/* SDL_TextureAccess */
	luaMagic::setValue(L, "TEXTUREACCESS_STATIC", (int)SDL_TEXTUREACCESS_STATIC, true);
	luaMagic::setValue(L, "TEXTUREACCESS_STREAMING", (int)SDL_TEXTUREACCESS_STREAMING, true);
	luaMagic::setValue(L, "TEXTUREACCESS_TARGET", (int)SDL_TEXTUREACCESS_TARGET, true);
	
	/* special handling */
	lua_pushcfunction(L, SDLuna_SetRenderTarget);
	lua_setfield(L, -2, "SetRenderTarget");
}

/* special handling */
static int SDLuna_SetRenderTarget(lua_State* L)
{
	SDL_Renderer* renderer = nullptr;
	SDL_Texture* texture = nullptr;
	if(luaL_checkudata(L, 1, UDATA_SDL_RENDERER))
	{
		renderer = *static_cast<SDL_Renderer**>(lua_touserdata(L, 1));
	}
	else
	{
		luaL_error(L, "bad argument #%d (%s expected, got %s)", 
					1, UDATA_SDL_RENDERER, luaL_typename(L, 1));
		return 0;
	}
	
	/* <texture> accept not only SDL_Texture* but also nil */
	if(lua_isnoneornil(L, 2))
	{
		texture = nullptr;
	}
	else
	{
		texture = *static_cast<SDL_Texture**>(luaL_checkudata(L, 2, UDATA_SDL_TEXTURE));
	}
	
	lua_pushinteger(L, SDL_SetRenderTarget(renderer, texture));
	return 1;
}