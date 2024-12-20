#include "SLO_GLTexture.h"
#include "SLO_SDLImage.h"
#include <SDL_opengl.h>

namespace GL_
{

Texture::Texture(std::shared_ptr<SDL_::Image> surface)
	: texRange_{}
	, textureName_(image2texture(surface, texRange_))
	, width_(surface->getWidth())
	, height_(surface->getHeight())
{
}

Texture::~Texture()
{
	::glDeleteTextures(1, &textureName_);
}

void Texture::update(std::shared_ptr<SDL_::Image> src)
{
	auto image = prepareTextureImage(src, texRange_);

	glBindTexture(GL_TEXTURE_2D, textureName_);
	glTexSubImage2D(GL_TEXTURE_2D, 0,
			0, 0,
			image->getWidth(), image->getHeight(),
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			image->getPixels());
}

void Texture::bind()
{
	::glBindTexture(GL_TEXTURE_2D, textureName_);
}

GLuint Texture::image2texture(std::shared_ptr<SDL_::Image> src, std::array<float, 4> &texRange)
{
	auto image = prepareTextureImage(src, texRange);

	// Create an OpenGL texture for the image
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGBA,
			image->getWidth(), image->getHeight(),
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			image->getPixels());

	return texture;
}

std::shared_ptr<SDL_::Image> Texture::prepareTextureImage(std::shared_ptr<SDL_::Image> src, std::array<float, 4> &texRange)
{
	// Use the surface width and height expanded to powers of 2
	const int w = power_of_two(src->getWidth());
	const int h = power_of_two(src->getHeight());
	texRange[0] = 0.0f;         // Min X
	texRange[1] = 0.0f;         // Min Y
	texRange[2] = static_cast<GLfloat>(src->getWidth()) / w;  // Max X
	texRange[3] = static_cast<GLfloat>(src->getHeight()) / h;  // Max Y

	auto image = std::make_shared<SDL_::Image>(w, h);

	// Save the alpha blending attributes
	const Color savedColor = src->getColorMod();
	const auto savedMode = src->getBlendMode();

	// Copy the surface into the GL texture image
	image->blit(src, {0, 0, src->getWidth(), src->getHeight()}, 0, 0);

	// Restore the alpha blending attributes
	src->setAlphaMod(savedColor.getAlpha());
	src->setBlendMode(savedMode);

	return image;
}

} // GL_
