#include "SLO_SDLTtfFont.h"
#include "SLO_SDLColor.h"
#include "SLO_SDLImage.h"

namespace SDL_
{

bool TtfFont::getTextSize(const char* text, Size *size) {
	bool result = false;
	if(size){
		int w, h;
		result = (::TTF_SizeUTF8(font_, text, &w, &h) == 0);
		if(result){
			size->set_width(w);
			size->set_height(h);
		}
	}
	return result;
}

std::shared_ptr<SDL_::Image> TtfFont::renderSolidText(const char* text, const Color& fg)
{
	std::shared_ptr<SDL_::Image> result;
	SDL_Surface *surface = ::TTF_RenderUTF8_Solid(font_, text, fg);
	if(surface){
		result.reset(new SDL_::Image(surface));
	}
	return result;
}

std::shared_ptr<SDL_::Image> TtfFont::renderShadedText(const char* text, const Color& fg, const Color& bg)
{
	std::shared_ptr<SDL_::Image> result;
	SDL_Surface *surface = ::TTF_RenderUTF8_Shaded(font_, text, fg, bg);
	if(surface){
		result.reset(new SDL_::Image(surface));
	}
	return result;
}

std::shared_ptr<SDL_::Image> TtfFont::renderBlendedText(const char* text, const Color& fg)
{
	std::shared_ptr<SDL_::Image> result;
	SDL_Surface *surface = ::TTF_RenderUTF8_Blended(font_, text, fg);
	if(surface){
		result.reset(new SDL_::Image(surface));
	}
	return result;
}

} // namespace SDL_
