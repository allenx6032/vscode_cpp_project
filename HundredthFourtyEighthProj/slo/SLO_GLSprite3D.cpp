#include "SLO_GLSprite3D.h"
#include "SLO_GLTexture.h"

namespace GL_
{

Sprite3D::Sprite3D(XOrigin xorigin, YOrigin yorigin)
	: Sprite(XOrigin::Center, YOrigin::Center)
{
}

Sprite3D::Sprite3D(std::shared_ptr<Texture> texture, XOrigin xorigin, YOrigin yorigin)
	: Sprite(texture, XOrigin::Center, YOrigin::Center)
{
}

Sprite3D::Sprite3D(std::shared_ptr<SDL_::Image> surface, XOrigin xorigin, YOrigin yorigin)
	: Sprite(surface, XOrigin::Center, YOrigin::Center)
{
}

Sprite3D::~Sprite3D()
{
}

} // GL_
