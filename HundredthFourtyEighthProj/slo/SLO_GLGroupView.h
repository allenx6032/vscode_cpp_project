#ifndef GLGROUPVIEW_H_
#define GLGROUPVIEW_H_

#include "SLO_GLView.h"
#include "SLO_SDLColor.h"
#include <list>

namespace GL_
{

class Texture;
class Sprite2D;
class GroupView: public View
{
public:
	GroupView();
	virtual ~GroupView() override;

	void removeChild(std::shared_ptr<View> child);
	void clearChild();
	std::list<std::shared_ptr<View>> &getChildren() { return children_; }
	void setBack(std::shared_ptr<Texture> back) ;

	static void addChild(std::shared_ptr<GroupView> parent, std::shared_ptr<View> child);

protected:
	virtual void onDraw() override;

private:
	void addChild(std::shared_ptr<View> child);

	std::list<std::shared_ptr<View>> children_;
	std::shared_ptr<Sprite2D> back_;
};

} // GL_

#endif // GLGROUPVIEW_H_
