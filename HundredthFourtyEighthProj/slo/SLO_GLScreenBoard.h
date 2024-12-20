#ifndef GRAPHICS_GLSCREENBOARD_H_
#define GRAPHICS_GLSCREENBOARD_H_

#include "SLO_GLSprite.h"

namespace GL_ {

class ScreenBoard: public Sprite {
public:
	using Sprite::Sprite;
	virtual ~ScreenBoard() override;

private:
	virtual void positioning() override;
};

} // namespace GL_

#endif // GRAPHICS_GLSCREENBOARD_H_
