#pragma once

#include "sl_transform.h"

#include "sl_config.h"

#ifdef USE_GLES
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#else
	#include <GLew/glew.h>
#endif

void sliSpriteInit();
void sliSpriteDestroy();
void sliSprite(Mat4 *modelview, Vec4 *color, GLuint texture, Vec2 *tiling, Vec2 *scroll);
