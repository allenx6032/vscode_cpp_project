#include "corange_static_object.h"

static_object* static_object_new() {
  static_object* s = malloc(sizeof(static_object));
  
  s->position = vec3_zero();
  s->rotation = quat_id();
  s->scale = vec3_one();
  
  s->active = true;
  s->recieve_shadows = true;
  s->cast_shadows = true;
  
  s->renderable = asset_hndl_null();
  s->collision_body = asset_hndl_null();
  
  return s;
}

void static_object_delete(static_object* s) {
  free(s);
}

mat4 static_object_world(static_object* s) {
  return mat4_world(s->position, s->scale, s->rotation);
}

mat3 static_object_world_normal(static_object* s) {
  return mat3_transpose(mat3_inverse(mat4_to_mat3(static_object_world(s))));
}