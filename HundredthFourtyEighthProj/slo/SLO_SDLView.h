#ifndef SDLVIEW_H_
#define SDLVIEW_H_

#include "SLO_Geometry.h"
#include "SLO_Misc.h"
#include <memory>

namespace SDL_
{

class Renderer;
class GroupView;
class View
{
public:
	UNCOPYABLE(View);
	View()
		: parent_()
		, pos_()
		, size_()
		, visible_(true)
	{
	}
	virtual ~View() = default;

	void draw(Renderer &);
	virtual void recalcLayout();

	void setParent(std::shared_ptr<GroupView> parent)	{ parent_ = parent; }
	std::shared_ptr<GroupView> getParent() const { return parent_.lock(); }
	void clearParent() { parent_.reset(); }

	void setPos(const Point &pos){ pos_ = pos; }
	void setXPos(const int x) { pos_.set_x_pos(x); }
	void setYPos(const int y) { pos_.set_y_pos(y); }
	const Point &getPos() const { return pos_; }
	int getXPos() const { return pos_.get_x_pos(); }
	int getYPos() const { return pos_.get_y_pos(); }
	int getAbsoluteXPos() const;
	int getAbsoluteYPos() const;
	Point getAbsolutePos() const
	{
		return Point(getAbsoluteXPos(), getAbsoluteYPos());
	}

	void alignLeft();
	void alignRight();
	void alignTop();
	void alignBottom();
	void centeringXPos();
	void centeringYPos();

	void setSize(const Size &size){ size_ = size; }
	void setWidth(const int width){ size_.w = width; }
	void setHeight(const int height){ size_.h = height; }
	void matchParentWidth();
	void matchParentHeight();
	void matchParentSize();
	const Size &getSize() const { return size_; }
	int getWidth() const { return size_.w; }
	int getHeight() const { return size_.h; }

	bool isVisible() const { return visible_; }
	void setVisiblity(bool visible) { visible_ = visible; }
	void show() { visible_ = true; }
	void hide() { visible_ = false; }

protected:
	virtual void onDraw(Renderer &renderer) = 0;

private:
	std::weak_ptr<GroupView> parent_;
	Point pos_;
	Size size_;
	bool visible_;
};

class ViewClip
{
	UNCOPYABLE(ViewClip);
public:
	ViewClip(View &view, Renderer &renderer);
	~ViewClip();

	bool isValid() const { return !clip_rect_.isEmpty(); }
private:
	Renderer &renderer_;
	Rect clip_rect_;
	const bool clip_enable_;
};

enum class ViewXAlign
{
	Left,
	Center,
	Right,
};

enum class ViewYAlign
{
	Top,
	Center,
	Bottom,
};

} // SDL_

#endif // SDLVIEW_H_
