#ifndef GRAPHICS_GLBILLBOARD_H_
#define GRAPHICS_GLBILLBOARD_H_

#include "SLO_GLSprite3D.h"

namespace GL_ {

class BillBoard: public Sprite3D {
public:
	using Sprite3D::Sprite3D;
	virtual ~BillBoard() override;

private:
	virtual void positioning() override;
};

} // namespace GL_

#endif // GRAPHICS_GLBILLBOARD_H_
