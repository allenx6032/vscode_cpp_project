#include "SLO_VerticalRotateMenu.h"
#include "SLO_GLSprite3D.h"
#include <SDL_opengl.h>
#include <iostream>

namespace GL_ {

VerticalRotateMenu::VerticalRotateMenu()
	: RotateMenu()
{
}

VerticalRotateMenu::~VerticalRotateMenu()
{
}

void VerticalRotateMenu::draw()
{
	auto &listMenuItem = getMenuItemList();
	if (listMenuItem.empty()) {
		return;
	}

	//std::cout << "vertical rotate menu current degrees = " << getDegrees() << std::endl;
	glRotated(getDegrees(), 1.0, 0.0, 0.0);

	const double deltaDegrees = 360.0 / listMenuItem.size();
	for (auto item : listMenuItem) {
		item->setXPos(0.0);
		item->setYPos(0.0);
		item->setZPos(getRadius());
		item->draw();
		glRotated(deltaDegrees, 1.0, 0.0, 0.0);
	}
}

} // namespace GL_
