#include "SLO_GLImageView.h"
#include "SLO_GLTexture.h"
#include "SLO_GLSprite2D.h"
#include "SLO_SDLImage.h"

namespace GL_
{

ImageView::ImageView()
	: View()
	, src_()
	, back_()
	, drawer_(std::make_shared<Sprite2D>())
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

void ImageView::setSrc(std::shared_ptr<SDL_::Image> image)
{
	src_ = std::make_shared<Texture>(image);
}

void ImageView::setBack(std::shared_ptr<SDL_::Image> image)
{
	back_ = std::make_shared<Texture>(image);
}

void ImageView::onDraw()
{
	if(back_){
		drawer_->setTexture(back_);
		drawer_->setPos(getPos());
		drawer_->setSize(getSize());
		drawer_->draw();
	}
	if(src_){
		const double sw = src_->getWidth();
		const double sh = src_->getHeight();
		const double vw = this->getWidth() - getPaddingLeft() - getPaddingRight();
		const double vh = this->getHeight() - getPaddingTop() - getPaddingBottom();

		drawer_->setTexture(src_);
		drawer_->setWidth(sw);
		drawer_->setHeight(sh);
		if (keepInbound_) {
			if (keepAspect_) {
				if (vh < sh || vw < sw) {
					const double yratio = vh / sh;
					const double xratio = vw / sw;

					if(xratio < yratio) {
						drawer_->setWidth(vw);
						drawer_->setHeight(sh * xratio);
					}
					else {
						drawer_->setWidth(sw * yratio);
						drawer_->setHeight(vh);
					}
				}
			}
			else {
				if (vw < sw) {
					drawer_->setWidth(vw);
				}
				if (vh < sh) {
					drawer_->setHeight(vh);
				}
			}
		}

		drawer_->setZPos(0.0);
		switch (alignY_) {
		case ViewYAlign::Top:
			drawer_->setYPos(getYPos() + getPaddingTop());
			break;
		case ViewYAlign::Center:
			drawer_->setYPos(getYPos() + getPaddingTop() + (vh - drawer_->getHeight()) / 2);
			break;
		case ViewYAlign::Bottom:
			drawer_->setYPos(getYPos() + vh - getPaddingBottom() - drawer_->getHeight());
			break;
		}

		switch(alignX_) {
		case ViewXAlign::Left:
			drawer_->setXPos(getXPos() + getPaddingLeft());
			break;
		case ViewXAlign::Center:
			drawer_->setXPos(getXPos() + getPaddingLeft() + (vw - drawer_->getWidth()) / 2);
			break;
		case ViewXAlign::Right:
			drawer_->setXPos(getXPos() + vw - getPaddingRight() - drawer_->getWidth());
			break;
		}
		drawer_->draw();
	}
}

void ImageView::setWrapContentSize()
{
	if(src_){
		const double sw = src_->getWidth();
		const double sh = src_->getHeight();
		setWidth(sw);
		setHeight(sh);
	}
}

} // GL_

