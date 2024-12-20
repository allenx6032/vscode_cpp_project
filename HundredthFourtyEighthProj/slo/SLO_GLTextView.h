#ifndef GLTEXTVIEW_H_
#define GLTEXTVIEW_H_

#include "SLO_GLImageView.h"
#include "SLO_SDLColor.h"
#include <string>

namespace SDL_
{
class TtfFont;
}

namespace GL_
{

class TextView : public ImageView
{
public:
	TextView()
		: ImageView()
		, text_()
	{
	}
	virtual ~TextView() override = default;

	void setText(SDL_::TtfFont &, const char * const, const Color &);
	void setText(SDL_::TtfFont &font, const std::string &text, const Color &color) { setText(font, text.c_str(), color); }
	const char *getText() const { return text_.c_str(); }

private:
	std::string text_;
};

} // GL_

#endif // GLTEXTVIEW_H_
