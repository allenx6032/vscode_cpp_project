#include "SLO_GLTextView.h"
#include "SLO_SDLTtfFont.h"
#include "SLO_SDLImage.h"
#include "SLO_GLTexture.h"

namespace GL_
{

void TextView::setText(SDL_::TtfFont &font, const char * const text, const Color &color)
{
	text_ = text;
	setSrc(font.renderBlendedText(text, color));
}

} // GL_
