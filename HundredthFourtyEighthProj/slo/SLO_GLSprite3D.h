#ifndef GRAPHICS_GLSPRITE3D_H_
#define GRAPHICS_GLSPRITE3D_H_

#include "SLO_GLSprite.h"

namespace GL_
{
class Sprite3D: public Sprite
{
public:
	Sprite3D(XOrigin = XOrigin::Center, YOrigin = YOrigin::Center);
	explicit Sprite3D(std::shared_ptr<Texture>, XOrigin = XOrigin::Center, YOrigin = YOrigin::Center);
	explicit Sprite3D(std::shared_ptr<SDL_::Image>, XOrigin = XOrigin::Center, YOrigin = YOrigin::Center);
	virtual ~Sprite3D() override;
};
} // GL_

#endif // GRAPHICS_GLSPRITE3D_H_
