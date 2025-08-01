/*
 * Copyright (c), Recep Aslantas.
 *
 * MIT License (MIT), htt../opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

/*
 Functions:
   CGLM_INLINE mat4s glms_lookat_lh_no(vec3s eye, vec3s center, vec3s up)
   CGLM_INLINE mat4s glms_look_lh_no(vec3s eye, vec3s dir, vec3s up)
   CGLM_INLINE mat4s glms_look_anyup_lh_no(vec3s eye, vec3s dir)
 */

#ifndef cglms_view_lh_no_h
#define cglms_view_lh_no_h

#include "cglm_common.h"
#include "cglm_types-struct.h"
#include "cglm_plane.h"
#include "cglm_cam.h"

/*!
 * @brief set up view matrix
 *        with a left-hand coordinate system and a
 *        clip-space of [-1, 1].
 *
 * NOTE: The UP vector must not be parallel to the line of sight from
 *       the eye point to the reference point
 *
 * @param[in]  eye    eye vector
 * @param[in]  center center vector
 * @param[in]  up     up vector
 * @returns    result matrix
 */
CGLM_INLINE
mat4s
glms_lookat_lh_no(vec3s eye, vec3s center, vec3s up) {
  mat4s dest;
  glm_lookat_lh_no(eye.raw, center.raw, up.raw, dest.raw);
  return dest;
}

/*!
 * @brief set up view matrix
 *        with a left-hand coordinate system and a
 *        clip-space of [-1, 1].
 *
 * convenient wrapper for lookat: if you only have direction not target self
 * then this might be useful. Because you need to get target from direction.
 *
 * NOTE: The UP vector must not be parallel to the line of sight from
 *       the eye point to the reference point
 *
 * @param[in]  eye    eye vector
 * @param[in]  dir    direction vector
 * @param[in]  up     up vector
 * @returns    result matrix
 */
CGLM_INLINE
mat4s
glms_look_lh_no(vec3s eye, vec3s dir, vec3s up) {
  mat4s dest;
  glm_look_lh_no(eye.raw, dir.raw, up.raw, dest.raw);
  return dest;
}

/*!
 * @brief set up view matrix
 *        with a left-hand coordinate system and a
 *        clip-space of [-1, 1].
 *
 * convenient wrapper for look: if you only have direction and if you don't
 * care what UP vector is then this might be useful to create view matrix
 *
 * @param[in]  eye    eye vector
 * @param[in]  dir    direction vector
 * @returns    result matrix
 */
CGLM_INLINE
mat4s
glms_look_anyup_lh_no(vec3s eye, vec3s dir) {
  mat4s dest;
  glm_look_anyup_lh_no(eye.raw, dir.raw, dest.raw);
  return dest;
}

#endif /* cglms_view_lh_no_h */
