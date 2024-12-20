#ifndef SDLIMAGEVIEW_H_
#define SDLIMAGEVIEW_H_

#include "SLO_SDLView.h"

namespace SDL_
{

class Renderer;
class Image;
class Texture;
class ImageView: public View
{
public:
	ImageView();
	virtual ~ImageView() override = default;

	void setSrc(Renderer &, std::shared_ptr<Image> image);
	void setSrc(std::shared_ptr<Texture> texture) { src_ = texture; }
	void setBack(Renderer &, std::shared_ptr<Image> image);
	void setBack(std::shared_ptr<Texture> texture) { back_ = texture; }

	int getPaddingLeft() const { return paddingLeft_; }
	int getPaddingRight() const { return paddingRight_; }
	int getPaddingTop() const { return paddingTop_; }
	int getPaddingBottom() const { return paddingBottom_; }
	void setPaddingLeft(int padding) { paddingLeft_ = padding; }
	void setPaddingRight(int padding) { paddingRight_ = padding; }
	void setPaddingTop(int padding) { paddingTop_ = padding; }
	void setPaddingBottom(int padding) { paddingBottom_ = padding; }
	void setPaddingAll(int padding)
	{
		setPaddingLeft(padding);
		setPaddingRight(padding);
		setPaddingTop(padding);
		setPaddingBottom(padding);
	}

	void keepAspect(bool keep) { keepAspect_ = keep; }
	void keepInbound(bool inbound) { keepInbound_ = inbound; }
	void setXAlign(ViewXAlign align) { alignX_ = align; }
	void setYAlign(ViewYAlign align) { alignY_ = align; }


protected:
	virtual void onDraw(Renderer &renderer) override;

private:
	std::shared_ptr<Texture> src_;
	std::shared_ptr<Texture> back_;
	int paddingLeft_;
	int paddingRight_;
	int paddingTop_;
	int paddingBottom_;
	ViewXAlign alignX_;
	ViewYAlign alignY_;
	bool keepAspect_;
	bool keepInbound_;
};

} // SDL_

#endif // SDLIMAGEVIEW_H_
