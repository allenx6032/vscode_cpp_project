#include "SLO_GLGroupView.h"
#include "SLO_GLSprite2D.h"
#include <SDL_opengl.h>

namespace GL_
{

GroupView::GroupView()
	: View()
	, children_()
	, back_(std::make_shared<GL_::Sprite2D>())
{
}

GroupView::~GroupView()
{
	clearChild();
}

void GroupView::onDraw()
{
	::glPushMatrix();
	::glTranslated(getXPos(), getYPos(), getZPos());
	if (back_) {
		back_->setXPos(0.0);
		back_->setYPos(0.0);
		back_->setZPos(0.0);
		back_->setWidth(getWidth());
		back_->setHeight(getHeight());
		back_->draw();
	}
	for(auto view : children_){
		view->draw();
	}
	::glPopMatrix();
}

void GroupView::addChild(std::shared_ptr<View> child)
{
	children_.push_back(child);
}

void GroupView::removeChild(std::shared_ptr<View> child)
{
	child->clearParent();
	children_.remove(child);
}

void GroupView::clearChild()
{
	for (auto child : children_) {
		child->clearParent();
	}
	children_.clear();
}

void GroupView::addChild(std::shared_ptr<GroupView> parent, std::shared_ptr<View> child)
{
	parent->addChild(child);
	child->setParent(parent);
}

void GroupView::setBack(std::shared_ptr<Texture> texture)
{
	back_->setTexture(texture);
}

} // GL_
