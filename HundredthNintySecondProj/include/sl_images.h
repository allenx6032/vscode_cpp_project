#pragma once

#include "sl_config.h"

#ifdef USE_GLES
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#else
	#include <GLew/glew.h>
#endif

GLuint loadOpenGLTexture(const char *filename);
