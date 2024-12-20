#include "SLO_SDLGroupView.h"
#include "SLO_SDLRenderer.h"

namespace SDL_
{

GroupView::~GroupView()
{
	clearChild();
}

void GroupView::onDraw(Renderer& renderer)
{
	ViewClip viewClip(*this, renderer);
	if(isVisible() && viewClip.isValid()){
		for(auto view : children_){
			if(view->isVisible()){
				view->draw(renderer);
			}
		}
	}
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

void GroupView::recalcLayout()
{
	for (auto child : children_) {
		child->recalcLayout();
	}
}

void GroupView::addChild(std::shared_ptr<GroupView> parent, std::shared_ptr<View> child)
{
	parent->addChild(child);
	child->setParent(parent);
}

} // SDL_
