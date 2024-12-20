#include "SLO_GLScreenBoard.h"
#include "SLO_GLTexture.h"
#include <SDL_opengl.h>

namespace GL_ {

ScreenBoard::~ScreenBoard()
{
}

void ScreenBoard::positioning()
{
	GLdouble m[16];
	::glPushMatrix();
	::glTranslated(getXPos() + getXOffset(), getYPos() + getYOffset(), getZPos() + getZOffset());
	::glGetDoublev(GL_MODELVIEW_MATRIX, m);
	::glPopMatrix();
	m[0] = 1.0; m[5] = 1.0; m[10] = -1.0;
	m[1] = m[2] = m[4] = m[6] = m[8] = m[9] = 0.0;
	::glLoadMatrixd(m);
	::glScaled(getWidth() * getScale(), getHeight() * getScale(), 1.0);
}

} // namespace GL_
