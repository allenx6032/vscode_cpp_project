#ifndef WIDGET_HORIZONTALROTATEMENU_H_
#define WIDGET_HORIZONTALROTATEMENU_H_

#include "SLO_RotateMenu.h"

namespace GL_
{

class Sprite3D;
class HorizontalRotateMenu : public RotateMenu<Sprite3D>
{
public:
	HorizontalRotateMenu();
	virtual ~HorizontalRotateMenu() override;

	virtual void draw() override;
};

}
#endif // WIDGET_HORIZONTALROTATEMENU_H_
