#ifndef WIDGET_VERTICALROTATEMENU_H_
#define WIDGET_VERTICALROTATEMENU_H_

#include "SLO_RotateMenu.h"

namespace GL_ {

class Sprite3D;
class VerticalRotateMenu : public RotateMenu<Sprite3D>
{
public:
	VerticalRotateMenu();
	virtual ~VerticalRotateMenu() override;

	virtual void draw() override;
};

} // namespace GL_

#endif // WIDGET_VERTICALROTATEMENU_H_
