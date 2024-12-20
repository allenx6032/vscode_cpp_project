#ifndef GRAPHICS_GLSPRITE2D_H_
#define GRAPHICS_GLSPRITE2D_H_

#include "SLO_GLSprite.h"

namespace GL_ {

class Sprite2D: public Sprite
{
public:
	Sprite2D(XOrigin = XOrigin::Left, YOrigin = YOrigin::Top);
	Sprite2D(std::shared_ptr<Texture>, XOrigin = XOrigin::Left, YOrigin = YOrigin::Top);
	Sprite2D(std::shared_ptr<SDL_::Image>, XOrigin = XOrigin::Left, YOrigin = YOrigin::Top);
	virtual ~Sprite2D() override;
	virtual std::array<float, 12> getVertexPointer() override;
};

} // namespace GL_

#endif // GRAPHICS_GLSPRITE2D_H_
