#include "SLO_GLSprite2D.h"

namespace GL_ {

Sprite2D::Sprite2D(XOrigin xorigin, YOrigin yorigin)
	: Sprite(xorigin, yorigin)
{

}

Sprite2D::Sprite2D(std::shared_ptr<Texture> texture, XOrigin xorigin, YOrigin yorigin)
	: Sprite(texture, xorigin, yorigin)
{
}

Sprite2D::Sprite2D(std::shared_ptr<SDL_::Image> image, XOrigin xorigin, YOrigin yorigin)
	: Sprite(image, xorigin, yorigin)
{
}

Sprite2D::~Sprite2D()
{
}

std::array<float, 12> Sprite2D::getVertexPointer()
{
	return {{
		getRight(), getBottom(), 0.0f,
		getRight(), getTop(), 0.0f,
		getLeft(), getTop(), 0.0f,
		getLeft(), getBottom(), 0.0f,
	}};
}

} // namespace GL_
