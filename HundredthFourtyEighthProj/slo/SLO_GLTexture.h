#ifndef GLTEXTURE_H_
#define GLTEXTURE_H_

#include "SLO_Misc.h"
#include <memory>
#include <array>

namespace SDL_
{
class Image;
}

namespace GL_
{

class Texture
{
	UNCOPYABLE(Texture);
public:
	Texture(std::shared_ptr<SDL_::Image>);
	~Texture();

	double getWidth() const { return width_; }
	double getHeight() const { return height_; }
	void update(std::shared_ptr<SDL_::Image>);

	void bind();
	const std::array<float, 4> &getTexRange() const { return texRange_; }

private:
	static uint32_t image2texture(std::shared_ptr<SDL_::Image>, std::array<float, 4> &);
	static std::shared_ptr<SDL_::Image> prepareTextureImage(std::shared_ptr<SDL_::Image>, std::array<float, 4> &);

	std::array<float, 4> texRange_;
	const uint32_t textureName_;
	const double width_;
	const double height_;
};

} // GL_

#endif // GLTEXTURE_H_
