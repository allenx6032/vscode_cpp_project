#ifndef GLSPRITE_H_
#define GLSPRITE_H_

#include "SLO_Misc.h"
#include "SLO_Vector3.h"
#include "SLO_Geometry.h"
#include <memory>
#include <array>
#include "SLO_Interpolator.h"

namespace SDL_
{
class Image;
}

namespace GL_ {
class Texture;
class Sprite
{
public:
	enum class XOrigin
	{
		Left,
		Center,
		Right,
	};

	enum class YOrigin
	{
		Top,
		Center,
		Bottom,
	};

	UNCOPYABLE(Sprite);
	Sprite(XOrigin, YOrigin);
	Sprite(std::shared_ptr<Texture>, XOrigin, YOrigin);
	Sprite(std::shared_ptr<SDL_::Image>, XOrigin, YOrigin);
	virtual ~Sprite();

	void setTexture(std::shared_ptr<Texture>);
	void setTexture(std::shared_ptr<SDL_::Image>);
	void clearTexture();
	std::shared_ptr<Texture> getTexture() const { return texture_; }

	void draw();

	const Vector3d &getPos() const { return pos_; }
	const double getXPos() const { return pos_.getX(); }
	const double getYPos() const { return pos_.getY(); }
	const double getZPos() const { return pos_.getZ(); }

	const Sized &getSize() const { return size_; }
	const double getWidth() const { return size_.getWidth(); }
	const double getHeight() const { return size_.getHeight(); }

	const Vector3d &getOffset() const { return offset_; }
	const double getXOffset() const { return offset_.getX(); }
	const double getYOffset() const { return offset_.getY(); }
	const double getZOffset() const { return offset_.getZ(); }

	void setPos(const Vector3d &pos) { pos_ = pos; }
	void setXPos(const double x) { pos_.setX(x); }
	void setYPos(const double y) { pos_.setY(y); }
	void setZPos(const double z) { pos_.setZ(z); }

	void setSize(const Sized &size) { size_ = size; }
	void setWidth(const double w) { size_.setWidth(w); }
	void setHeight(const double h) { size_.setHeight(h); }

	void setOffset(const Vector3d &offset) { offset_ = offset; }
	void setXOffset(const double x) { offset_.setX(x); }
	void setYOffset(const double y) { offset_.setY(y); }
	void setZOffset(const double z) { offset_.setZ(z); }

	void setAlpha(uint8_t alpha) { alpha_ = alpha; }
	uint8_t getAlpha() const { return alpha_; }

	void setScale(double scale) { scale_ = scale; }
	double getScale() const { return scale_; }

	virtual std::array<float, 12> getVertexPointer();
	std::array<float, 8> getTexCoord();

protected:
	float getLeft() const { return left_; }
	float getRight() const { return right_; }
	float getTop() const { return top_; }
	float getBottom() const { return bottom_; }

private:
	virtual void positioning();

	std::shared_ptr<Texture> texture_;
	Vector3d pos_;
	Vector3d offset_;
	Sized size_;
	uint8_t alpha_;
	double scale_;
	float left_;
	float right_;
	float top_;
	float bottom_;
	//ExInterpolator scaler_;
};

} // GL_

#endif // GLSPRITE_H_
