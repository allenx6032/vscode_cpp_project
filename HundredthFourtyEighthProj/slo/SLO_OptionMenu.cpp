#include "SLO_OptionMenu.h"
#include "SLO_GLTexture.h"
#include "SLO_SDLImage.h"
#include "SLO_GLSprite2D.h"
#include "SLO_Application.h"
#include <SDL_opengl.h>

OptionMenu::OptionMenu(double width, double height)
	: listIcon_()
	, back_(std::make_shared<GL_::Sprite2D>(GL_::Sprite::XOrigin::Left, GL_::Sprite::YOrigin::Bottom))
	, x_(0.0)
{
	auto image = std::make_shared<SDL_::Image>(64, 64);
	image->fillRect({0, 0, 0, 64});
	back_->setTexture(image);
	back_->setWidth(width);
	back_->setHeight(height);
}

OptionMenu::~OptionMenu()
{
}

void OptionMenu::clear()
{
	listIcon_.clear();
}

void OptionMenu::addIcon(std::shared_ptr<GL_::Sprite> icon)
{
	listIcon_.push_back(icon);
}

bool OptionMenu::action(uint32_t tick)
{
	return false;
}

void OptionMenu::draw()
{
	::glPushMatrix();
	back_->draw();
	::glTranslated(back_->getWidth() / 2 + x_, back_->getYPos(), back_->getZPos());
	for (auto icon : listIcon_) {
		icon->draw();
	}
	::glPopMatrix();
}

void OptionMenu::setWidth(double width)
{
	back_->setWidth(width);
}

double OptionMenu::getWidth() const
{
	return back_->getWidth();
}

void OptionMenu::setHeight(double height)
{
	back_->setHeight(height);
}

double OptionMenu::getHeight() const
{
	return back_->getHeight();
}

//void OptionMenu::setXPos(double x)
//{
//	x_ = x;
//}
//
//double OptionMenu::getXPos() const
//{
//	return x_;
//}

void OptionMenu::setYPos(double y)
{
	back_->setYPos(y);
}

double OptionMenu::getYPos() const
{
	return back_->getYPos();
}

void OptionMenu::moveCursor(int pos, bool animation)
{
	if (animation) {
		//xpositioner_.start(x_, -listIcon_[pos]->getXPos(), 200, Application::getTickCount());
		//xpositioner_.setAction([this](double value){ x_ = value; });
	}
	else {
		x_ = -listIcon_[pos]->getXPos();
	}
}
