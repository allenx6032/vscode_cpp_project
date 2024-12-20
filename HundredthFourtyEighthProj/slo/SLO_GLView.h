#ifndef GLVIEW_H_
#define GLVIEW_H_

#include "SLO_Geometry.h"
#include "SLO_Vector3.h"
#include "SLO_Misc.h"
#include <memory>

namespace GL_
{
class Sprite2D;
class GroupView;
class View
{
public:
	UNCOPYABLE(View);
	View();
	virtual ~View() = default;

	void draw();

	void setParent(std::shared_ptr<GroupView> parent)	{ parent_ = parent; }
	std::shared_ptr<GroupView> getParent() const { return parent_.lock(); }
	void clearParent() { parent_.reset(); }

	const Vector3d &getPos() const { return pos_; }
	double getXPos() const { return pos_.getX(); }
	double getYPos() const { return pos_.getY(); }
	double getZPos() const { return pos_.getZ(); }

	void setPos(const Vector3d &pos) { pos_ = pos; }
	void setXPos(const double x) { pos_.setX(x); }
	void setYPos(const double y) { pos_.setY(y); }
	void setZPos(const double z) { pos_.setZ(z); }

	const Sized &getSize() const { return size_; }
	double getWidth() const { return size_.getWidth(); }
	double getHeight() const { return size_.getHeight(); }

	void setSize(const Sized &size) { size_ = size; }
	void setWidth(const double w) { size_.setWidth(w); }
	void setHeight(const double h) { size_.setHeight(h); }

	void alignLeft();
	void alignRight();
	void alignTop();
	void alignBottom();
	void centeringXPos();
	void centeringYPos();

	void matchParentWidth();
	void matchParentHeight();
	void matchParentSize();

	bool isVisible() const { return visible_; }
	void setVisiblity(bool visible) { visible_ = visible; }
	void show() { visible_ = true; }
	void hide() { visible_ = false; }

protected:
	virtual void onDraw() = 0;

private:
	std::weak_ptr<GroupView> parent_;
	Vector3d pos_;
	Sized size_;
	bool visible_;
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

} // GL_

#endif // GLVIEW_H_
