/* Freetype GL - A C OpenGL Freetype engine
 *
 * Distributed under the OSI-approved BSD 2-Clause License.  See accompanying
 * file `LICENSE` for more details.
 */
#ifndef __FREETYPE_GL_H__
#define __FREETYPE_GL_H__

/* Mandatory */
#include "ftgl-platform.h"
#include "ftgl-opengl.h"
#include "ftgl-vec234.h"
#include "ftgl-vector.h"
#include "ftgl-texture-atlas.h"
#include "ftgl-texture-font.h"
#include "ftgl-utils.h"

#ifdef IMPLEMENT_FREETYPE_GL
#include "ftgl-platform.c"
#include "ftgl-texture-atlas.c"
#include "ftgl-texture-font.c"
#include "ftgl-vector.c"
#include "ftgl-utf8-utils.c"
#include "ftgl-distance-field.c"
#include "ftgl-edtaa3func.c"
#include "ftgl-utils.c"
#endif

#ifdef __cplusplus
#ifndef NOT_USING_FT_GL_NAMESPACE
using namespace ftgl;
#endif /* NOT_USING_FT_GL_NAMESPACE */
#endif /* __cplusplus */

#endif /* FREETYPE_GL_H */
