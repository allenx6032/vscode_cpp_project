#include "SLO_SDLTexture.h"
#include "SLO_SDLImage.h"
#include "SLO_SDLRenderer.h"

namespace SDL_
{

Texture::Texture(const Renderer &renderer, const Image &surface)
	: texture_(::SDL_CreateTextureFromSurface(renderer.get(), surface.get()))
	, width_(surface.getWidth())
	, height_(surface.getHeight())
{
}


Texture::Texture(const Renderer &renderer, Uint32 format, int access, int w, int h)
	: texture_(::SDL_CreateTexture(renderer.get(), format, access, w, h))
	, width_(w)
	, height_(h)
{
}

Texture::~Texture()
{
	if (texture_) {
		::SDL_DestroyTexture(texture_);
	}
}

} // SDL_
