#include "SLO_SDLImageView.h"
#include "SLO_SDLTexture.h"
#include "SLO_SDLImage.h"
#include "SLO_SDLRenderer.h"

namespace SDL_
{

ImageView::ImageView()
	: View()
	, src_()
	, back_()
	, paddingLeft_()
	, paddingRight_()
	, paddingTop_()
	, paddingBottom_()
	, alignX_(ViewXAlign::Center)
	, alignY_(ViewYAlign::Center)
	, keepAspect_(true)
	, keepInbound_(true)
{
}

void ImageView::setSrc(Renderer &renderer, std::shared_ptr<Image> image)
{
	src_ = std::make_shared<Texture>(renderer, *image);
}

void ImageView::setBack(Renderer &renderer, std::shared_ptr<Image> image)
{
	back_ = std::make_shared<Texture>(renderer, *image);
}

void ImageView::onDraw(Renderer& renderer)
{
	Rect dstrect;
	if(back_){
		dstrect.setXPos(this->getAbsoluteXPos());
		dstrect.setYPos(this->getAbsoluteYPos());
		dstrect.setWidth(this->getWidth());
		dstrect.setHeight(this->getHeight());
		renderer.copy(back_, nullptr, &dstrect);
	}
	if(src_){
		const double sw = src_->getWidth();
		const double sh = src_->getHeight();
		const double vw = this->getWidth();
		const double vh = this->getHeight();

		dstrect.setWidth(vw);
		dstrect.setHeight(vh);
		if (keepInbound_) {
			if (keepAspect_) {
				if (sw < sh) {
					if (vh < sh) {
						const double ratio = vh / sh;
						dstrect.setWidth(sw * ratio);
						dstrect.setHeight(vh);
					}
				}
				else {
					if (vw < sw) {
						const double ratio = vw / sw;
						dstrect.setWidth(vw);
						dstrect.setHeight(sh * ratio);
					}
				}
			}
			else {
				if (vw < sw) {
					dstrect.setWidth(vw);
				}
				if (vh < sh) {
					dstrect.setHeight(vh);
				}
			}
		}

		switch (alignY_) {
		case ViewYAlign::Top:
			dstrect.setYPos(getYPos());
			break;
		case ViewYAlign::Center:
			dstrect.setYPos(getYPos() + (vh - dstrect.getHeight()) / 2);
			break;
		case ViewYAlign::Bottom:
			dstrect.setYPos(getYPos() + vh - dstrect.getHeight());
			break;
		}

		switch(alignX_) {
		case ViewXAlign::Left:
			dstrect.setXPos(getXPos());
			break;
		case ViewXAlign::Center:
			dstrect.setXPos(getXPos() + (vw - dstrect.getWidth()) / 2);
			break;
		case ViewXAlign::Right:
			dstrect.setXPos(getXPos() + vw - dstrect.getWidth());
			break;
		}
		renderer.copy(src_, nullptr, &dstrect);
	}
}

} // SDL_
