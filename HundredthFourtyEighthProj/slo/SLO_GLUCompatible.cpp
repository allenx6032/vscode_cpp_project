#include "SLO_GLUCompatible.h"
#include "SLO_Vector3.h"

void gluLookAtCompatible(GLfloat eyeX, GLfloat eyeY, GLfloat eyeZ,
		GLfloat lookAtX, GLfloat lookAtY, GLfloat lookAtZ,
		GLfloat upX, GLfloat upY, GLfloat upZ)
{
	auto z = Vector3f(eyeX-lookAtX, eyeY-lookAtY, eyeZ-lookAtZ);
	z.normalize();
	auto y = Vector3f(upX, upY, upZ);
	auto x = y % z;
	y = z % x;
	x.normalize();
	y.normalize();
	// mat is given transposed so OpenGL can handle it.
	float mat[] =
					 {x.getX(), y.getX(),   z.getX(),   0,
					 x.getY(),  y.getY(),   z.getY(),   0,
					 x.getZ(),  y.getZ(),   z.getZ(),   0,
					 0,     0,   	   0,      1};
	glMultMatrixf(mat);
	glTranslated(-eyeX, -eyeY, -eyeZ);
}

void gluPerspectiveCompatible( GLdouble fovy, GLdouble aspect,
		GLdouble zNear, GLdouble zFar )
{
	const auto PI = 3.14159265358979323846;

	const auto ymax = zNear * tan( fovy * PI / 360.0 );
	const auto ymin = -ymax;

	const auto xmin = ymin * aspect;
	const auto xmax = ymax * aspect;

	//std::cout << "xmin = " << xmin << ", xmax = " << xmax << ", ymin = " << ymin << ", ymax = " << ymax << std::endl;
	::glFrustum( xmin, xmax, ymin, ymax, zNear, zFar );
	//::glOrtho( xmin, xmax, ymin, ymax, zNear, zFar );
}
