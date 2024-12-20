#include "SLO_SDLTextView.h"
#include "SLO_SDLRenderer.h"
#include "SLO_SDLTtfFont.h"
#include "SLO_SDLImage.h"
#include "SLO_SDLTexture.h"

namespace SDL_
{

void TextView::setText(Renderer &renderer, TtfFont &font, const char * const text, const Color &color)
{
	text_ = text;
	setSrc(renderer, font.renderBlendedText(text, color));
}

} // namespace SDL_
