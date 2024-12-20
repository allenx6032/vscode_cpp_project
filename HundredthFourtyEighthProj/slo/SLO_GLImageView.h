#ifndef GLIMAGEVIEW_H_
#define GLIMAGEVIEW_H_

#include "SLO_GLView.h"

namespace SDL_
{
class Image;
}

namespace GL_
{

class Sprite2D;
class Texture;
class ImageView: public View
{
public:
	ImageView();
	virtual ~ImageView() override = default;

	void setSrc(std::shared_ptr<SDL_::Image> image);
	void setSrc(std::shared_ptr<Texture> texture) { src_ = texture; }
	std::shared_ptr<Texture> getSrc() const { return src_; }

	void setBack(std::shared_ptr<SDL_::Image> image);
	void setBack(std::shared_ptr<Texture> texture) { back_ = texture; }
	std::shared_ptr<Texture> getBack() const { return back_; }

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
	void setWrapContentSize();

protected:
	virtual void onDraw() override;

private:
	std::shared_ptr<Texture> src_;
	std::shared_ptr<Texture> back_;
	std::shared_ptr<Sprite2D> drawer_;
	int paddingLeft_;
	int paddingRight_;
	int paddingTop_;
	int paddingBottom_;
	ViewXAlign alignX_;
	ViewYAlign alignY_;
	bool keepAspect_;
	bool keepInbound_;
};

} // GL_

#endif // GLIMAGEVIEW_H_
