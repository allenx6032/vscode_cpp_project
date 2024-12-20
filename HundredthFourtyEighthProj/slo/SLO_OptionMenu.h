#ifndef SCENE_MENU_OPTIONMENU_H_
#define SCENE_MENU_OPTIONMENU_H_

#include "SLO_Misc.h"
#include <vector>
#include <memory>

namespace GL_
{
class Sprite;
class Texture;
}

class OptionMenu
{
public:
	UNCOPYABLE(OptionMenu);
	OptionMenu(double width, double height);
	~OptionMenu();

	void clear();
	void addIcon(std::shared_ptr<GL_::Sprite>);
	bool action(uint32_t);
	void draw();
	void setWidth(double width);
	double getWidth() const;
	void setHeight(double height);
	double getHeight() const;
//	void setXPos(double x);
//	double getXPos() const;
	void setYPos(double y);
	double getYPos() const;

	void moveCursor(int, bool);

private:
	std::vector<std::shared_ptr<GL_::Sprite>> listIcon_;
	std::shared_ptr<GL_::Sprite> back_;
	double x_;
};

#endif // SCENE_MENU_OPTIONMENU_H_
