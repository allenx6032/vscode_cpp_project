#include "SLO_GLBillBoard.h"
#include "SLO_GLTexture.h"
#include <SDL_opengl.h>

namespace GL_ {

BillBoard::~BillBoard()
{
}

void BillBoard::positioning()
{
	GLdouble m[16];
	::glPushMatrix();
	::glTranslated(getXPos() + getXOffset(), getYPos() + getYOffset(), getZPos() + getZOffset());
	::glGetDoublev(GL_MODELVIEW_MATRIX, m);
	::glPopMatrix();
#if 1
	m[0] = 1.0f; m[5] = 1.0f; m[10] = -1.0f;
	m[1] = m[2] = m[4] = m[6] = m[8] = m[9] = 0.0f;
#else
    // Z 軸
	{
		m[8] = m[12];
		m[9] = m[13];
		m[10] = m[14];
		auto l = sqrt(m[8] * m[8] + m[9] * m[9] + m[10] * m[10]);
		m[8] /= l;
		m[9] /= l;
		m[10] /= l;
	}

    // X 軸 = (0, 1, 0) × Z 軸
	{
		m[0] = m[14];
		m[1] = 0.0;
		m[2] = -m[12];
		auto l = sqrt(m[0] * m[0] + m[1] * m[1] + m[2] * m[2]);
		m[0] /= l;
		m[1] /= l;
		m[2] /= l;
	}

    // Y 軸 = Z 軸 × X 軸
	m[4] = 0.0;//m[9] * m[2] - m[10] * m[1];
	m[5] = m[10] * m[0] - m[8] * m[2];
	m[6] = 0.0;//m[8] * m[1] - m[9] * m[0];
#endif
	::glLoadMatrixd(m);
	::glScaled(getWidth() * getScale(), getHeight() * getScale(), 1.0);
}

} // namespace GL_
