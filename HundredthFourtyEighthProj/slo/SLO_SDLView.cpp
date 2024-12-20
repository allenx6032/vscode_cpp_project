#include "SLO_SDLView.h"
#include "SLO_SDLGroupView.h"
#include "SLO_SDLRenderer.h"

namespace SDL_
{

void View::draw(Renderer &renderer)
{
	if (isVisible()) {
		onDraw(renderer);
	}
}

int View::getAbsoluteXPos() const
{
	int x = getXPos();// + getMarginLeft();
	auto parent = getParent();
	if (parent) {
		x += parent->getAbsoluteXPos();
	}
	return x;
}

int View::getAbsoluteYPos() const
{
	int y = getYPos();// + getMarginTop();
	auto parent = getParent();
	if (parent) {
		y += parent->getAbsoluteYPos();
	}
	return y;
}

void View::alignLeft()
{
	setXPos(0);
}

void View::alignRight()
{
	auto parent = getParent();
	if (parent) {
		setXPos(parent->getWidth() - this->getWidth());
	}
}

void View::alignTop()
{
	setYPos(0);
}

void View::alignBottom()
{
	auto parent = getParent();
	if (parent) {
		setYPos(parent->getHeight() - this->getHeight());
	}
}

void View::centeringXPos()
{
	auto parent = getParent();
	if (parent) {
		setXPos((parent->getWidth()/2) - (this->getWidth()/2));
	}
}

void View::centeringYPos()
{
	auto parent = getParent();
	if (parent) {
		setYPos((parent->getHeight()/2) - (this->getHeight()/2));
	}
}

void View::recalcLayout()
{
}

void View::matchParentWidth()
{
	auto parent = getParent();
	if (parent) {
		setWidth(parent->getWidth());
	}
}

void View::matchParentHeight()
{
	auto parent = getParent();
	if (parent) {
		setHeight(parent->getHeight());
	}
}

void View::matchParentSize()
{
	auto parent = getParent();
	if (parent) {
		setSize(parent->getSize());
	}
}

ViewClip::ViewClip(View &view, Renderer &renderer)
	: renderer_(renderer)
	, clip_rect_()
	, clip_enable_(renderer.isClipEnabled())
{
	const Rect view_rect(view.getAbsolutePos(), view.getSize());
	if(clip_enable_){
		clip_rect_ = renderer.getClipRect();
		renderer.setClipRect(clip_rect_ & view_rect);
	}
	else{
		clip_rect_ = view_rect;
		renderer.setClipRect(view_rect);
	}
}

ViewClip::~ViewClip() {
	if(clip_enable_){
		renderer_.setClipRect(clip_rect_);
	}
	else{
		renderer_.clearClipRect();
	}
}

} // SDL_
