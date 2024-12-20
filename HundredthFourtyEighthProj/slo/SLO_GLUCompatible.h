#ifndef GLUCOMPATIBLE_H_
#define GLUCOMPATIBLE_H_

#include <SDL_opengl.h>

void gluLookAtCompatible(GLfloat eyeX, GLfloat eyeY, GLfloat eyeZ,
		GLfloat lookAtX, GLfloat lookAtY, GLfloat lookAtZ,
		GLfloat upX, GLfloat upY, GLfloat upZ);

void gluPerspectiveCompatible( GLdouble fovy, GLdouble aspect,
		GLdouble zNear, GLdouble zFar );



#endif // GLUCOMPATIBLE_H_
