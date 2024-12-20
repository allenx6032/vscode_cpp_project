#include "SLO_GLView.h"
#include "SLO_GLGroupView.h"
#include "SLO_Interpolator.h"

namespace GL_ {

View::View()
	: parent_()
	, visible_(true)
{
}

//int View::getAbsoluteXPos() const
//{
//	int x = getXPos();// + getMarginLeft();
//	auto parent = getParent();
//	if (parent) {
//		x += parent->getAbsoluteXPos();
//	}
//	return x;
//}

//int View::getAbsoluteYPos() const
//{
//	int y = getYPos();// + getMarginTop();
//	auto parent = getParent();
//	if (parent) {
//		y += parent->getAbsoluteYPos();
//	}
//	return y;
//}

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

void View::draw()
{
	if (isVisible()) {
		onDraw();
	}
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

} // GL_
