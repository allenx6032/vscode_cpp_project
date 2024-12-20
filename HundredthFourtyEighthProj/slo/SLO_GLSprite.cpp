#include "SLO_GLSprite.h"
#include "SLO_GLTexture.h"
#include "SLO_SDLImage.h"
#include <SDL_opengl.h>

namespace GL_
{

Sprite::Sprite(XOrigin xorigin, YOrigin yorigin)
	: texture_()
	, pos_()
	, offset_()
	, size_()
	, alpha_(255)
	, scale_(1.0)
	, left_()
	, right_()
	, top_()
	, bottom_()
{
	switch(xorigin)
	{
	default:
	case XOrigin::Left:
		left_ = 0.0f;
		right_ = 1.0f;
		break;
	case XOrigin::Center:
		left_ = -0.5f;
		right_ = 0.5f;
		break;
	case XOrigin::Right:
		left_ = -1.0f;
		right_ = 0.0f;
		break;
	}

	switch(yorigin)
	{
	default:
	case YOrigin::Top:
		top_ = 0.0f;
		bottom_ = 1.0f;
		break;
	case YOrigin::Center:
		top_ = -0.5f;
		bottom_ = 0.5f;
		break;
	case YOrigin::Bottom:
		top_ = -1.0f;
		bottom_ = 0.0f;
		break;
	}
}

Sprite::Sprite(std::shared_ptr<Texture> texture, XOrigin xorigin, YOrigin yorigin)
	: Sprite(xorigin, yorigin)
{
	setTexture(texture);
}

Sprite::Sprite(std::shared_ptr<SDL_::Image> image, XOrigin xorigin, YOrigin yorigin)
	: Sprite(xorigin, yorigin)
{
	setTexture(image);
}

Sprite::~Sprite()
{
}

void Sprite::setTexture(std::shared_ptr<Texture> texture)
{
	texture_ = texture;
}

void Sprite::setTexture(std::shared_ptr<SDL_::Image> surface)
{
	texture_ = std::make_shared<Texture>(surface);
}

void Sprite::clearTexture()
{
	texture_.reset();
}

void Sprite::draw()
{
	if (!texture_) {
		return;
	}

	const std::array<float, 12> vertex = getVertexPointer();
	const std::array<float, 8> texCoord = getTexCoord();

	const auto alpha = getAlpha();
	const GLubyte colors[] = {
		255, 255, 255, alpha,
		255, 255, 255, alpha,
		255, 255, 255, alpha,
		255, 255, 255, alpha,
	};

	texture_->bind();
	::glPushMatrix();
	positioning();
	::glEnableClientState(GL_VERTEX_ARRAY);
	::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	::glEnableClientState(GL_COLOR_ARRAY);
	::glVertexPointer(3, GL_FLOAT, 0, vertex.data());
	::glTexCoordPointer(2, GL_FLOAT, 0, texCoord.data());
	::glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);
	::glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	::glEnable(GL_TEXTURE_2D);
	::glDrawArrays(GL_QUADS, 0, 4);
//	::glBegin(GL_QUADS);
//	::glTexCoord2f(0, 0); glVertex3f(-1.0f, -1.0f, 0);
//	::glTexCoord2f(0, 1); glVertex3f(-1.0f, 1.0f, 0);
//	::glTexCoord2f(1, 1); glVertex3f(1.0f, 1.0f, 0);
//	::glTexCoord2f(1, 0); glVertex3f(1.0f, -1.0f, 0);
//	::glEnd();
	::glDisable(GL_TEXTURE_2D);
	::glEnableClientState(GL_COLOR_ARRAY);
	::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	::glDisableClientState(GL_VERTEX_ARRAY);
	::glPopMatrix();
}

std::array<float, 12> Sprite::getVertexPointer()
{
	return {{
#if 0
		getLeft(), getTop(), 0.0f,
		getLeft(), getBottom(), 0.0f,
		getRight(), getBottom(), 0.0f,
		getRight(), getTop(), 0.0f,
#elif 1
		getRight(), getTop(), 0.0f,
		getRight(), getBottom(), 0.0f,
		getLeft(), getBottom(), 0.0f,
		getLeft(), getTop(), 0.0f,
#else
		getRight(), getBottom(), 0.0f,
		getRight(), getTop(), 0.0f,
		getLeft(), getTop(), 0.0f,
		getLeft(), getBottom(), 0.0f,
#endif
	}};
}

std::array<float, 8> Sprite::getTexCoord()
{
	if (!texture_) {
		return {{0, 0, 0, 0, 0, 0, 0, 0}};
	}

	const auto texRange = texture_->getTexRange();
	return {{
#if 0
			texRange[0], texRange[1],
			texRange[0], texRange[3],
			texRange[2], texRange[3],
			texRange[2], texRange[1],
#else // こっちにするとテクスチャの上下が逆になるよ
			texRange[2], texRange[3],
			texRange[2], texRange[1],
			texRange[0], texRange[1],
			texRange[0], texRange[3],
#endif
	}};
}

void Sprite::positioning()
{
	::glTranslated(getXPos() + getXOffset(), getYPos() + getYOffset(), getZPos() + getZOffset());
	::glScaled(getWidth() * getScale(), getHeight() * getScale(), 1.0);
}

} // GL_
