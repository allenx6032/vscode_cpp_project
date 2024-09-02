#pragma once

#include "mat4.h"

typedef struct Renderer Renderer;

/// A basic type which provide a render function pointer
typedef struct {
  int (*render)(void *this, Mat4 transform, Renderer *renderer);
} Renderable;
