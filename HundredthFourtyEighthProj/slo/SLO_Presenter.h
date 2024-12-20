#ifndef SCENE_PRESENTER_H_
#define SCENE_PRESENTER_H_

#include "SLO_Misc.h"
#include <memory>

namespace GL_
{
class Sprite;
class BillBoard;
class Sprite2D;
class Sprite3D;
class TextView;
class GroupView;
class ScreenView;
}

class Scene;
class Presenter
{
public:
	UNCOPYABLE(Presenter);
	Presenter(Scene &);
	virtual ~Presenter();

	void update();
	void finish();
	void quit();

	virtual void exposed();

	bool isIdling() const { return idling_; }
	std::shared_ptr<GL_::ScreenView> getRootView() { return root_; }

protected:
	void setIdling(bool idling) { idling_ = idling; }

private:
	Scene &scene_;
	std::shared_ptr<GL_::ScreenView> root_;
	bool idling_;
};

#endif // SCENE_MENU_MENUPRESENTER_H_
