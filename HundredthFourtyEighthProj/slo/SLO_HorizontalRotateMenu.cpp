#include "SLO_HorizontalRotateMenu.h"
#include "SLO_GLSprite3D.h"
#include <SDL_opengl.h>
#include <iostream>

namespace GL_ {

HorizontalRotateMenu::HorizontalRotateMenu()
	: RotateMenu()
{
}

HorizontalRotateMenu::~HorizontalRotateMenu()
{
}

void HorizontalRotateMenu::draw()
{
	auto &listMenuItem = getMenuItemList();
	if (listMenuItem.empty()) {
		return;
	}

	const double deltaDegrees = MAX_DEGREES / listMenuItem.size();
	int i = 0;
	for (auto item : listMenuItem) {
		double radian = degrees2radian(getDegrees() + 90.0 - deltaDegrees * i);
		item->setXPos(getRadius() * std::cos(radian));
		item->setYPos(0.0);
		item->setZPos(getRadius() * std::sin(radian));
		item->draw();
		++i;
	}

	//std::cout << "hrizontal rotate menu current degrees = " << getDegrees() << std::endl;

	std::vector<std::shared_ptr<GL_::Sprite3D>> tmpMenuList(listMenuItem.begin(), listMenuItem.end());
	std::sort(tmpMenuList.begin(), tmpMenuList.end(), [](auto lh, auto rh) ->int {
		return (lh->getZPos() < rh->getZPos());
		//return (lh->getZPos() > rh->getZPos());
	});

	for (auto item : tmpMenuList) {
		item->draw();
	}
}

}
