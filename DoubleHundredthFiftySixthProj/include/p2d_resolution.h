/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#ifndef P2D_RESOLUTION_H
#define P2D_RESOLUTION_H

#include <Lilith.h>

#include "p2d_export.h"
#include "p2d_core.h"
#include "p2d_types.h"

P2D_API void p2d_object_step(struct p2d_object *object, float delta_time, int iterations);

P2D_API void p2d_resolve_collision(struct p2d_collision_manifold *manifold);

#endif // P2D_RESOLUTION_H
