#ifndef SDLTEXTVIEW_H_
#define SDLTEXTVIEW_H_

#include "SLO_SDLImageView.h"
#include "SLO_SDLColor.h"
#include <memory>
#include <string>


namespace SDL_
{

class TtfFont;
class TextView : public ImageView
{
public:
	TextView()
		: ImageView()
		, text_()
	{
	}
	virtual ~TextView() override = default;

	void setText(Renderer &renderer, TtfFont &, const char * const, const Color &);
	void setText(Renderer &renderer, TtfFont &font, const std::string &text, const Color &color) { setText(renderer, font, text.c_str(), color); }
	const char *getText() const { return text_.c_str(); }

protected:
	virtual void onDraw(Renderer &renderer) override;

private:
	std::string text_;
};

} // SDL_

#endif // SDLTEXTVIEW_H_
