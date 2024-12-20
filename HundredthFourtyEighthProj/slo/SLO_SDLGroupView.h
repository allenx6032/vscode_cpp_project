#ifndef SDLGROUPVIEW_H_
#define SDLGROUPVIEW_H_

#include "SLO_SDLView.h"
#include <list>

namespace SDL_
{

class Renderer;
class GroupView: public View
{
public:
	GroupView()
		: View()
		, children_()
	{
	}
	virtual ~GroupView() override;

	virtual void addChild(std::shared_ptr<View> child);
	virtual void removeChild(std::shared_ptr<View> child);
	void clearChild();
	std::list<std::shared_ptr<View>> &getChildren() { return children_; }
	virtual void recalcLayout() override;

	static void addChild(std::shared_ptr<GroupView> parent, std::shared_ptr<View> child);

protected:
	virtual void onDraw(Renderer &renderer) override;

private:
	std::list<std::shared_ptr<View>> children_;
};

} // SDL_

#endif // GROUPVIEW_H_
