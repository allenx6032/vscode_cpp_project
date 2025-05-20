/* Deccan Game Engine - C11 2D SDL Game Engine.
 * Copyright 2020, 2021 Ayush Bardhan Tripathy
 *
 * This software is licensed under MIT License.
 * See LICENSE.md included with this package for more info.
 */

#pragma once

#include <deccan_flecs.h>
#include <deccan_transform.h>
#include <deccan_rigid_body.h>

void physics_sync_gameobject(deccan_comp_transform_t *transform, deccan_comp_rigid_body_t *rigid_body);
void physics_sync_physicsbody(deccan_comp_transform_t *transform, deccan_comp_rigid_body_t *rigid_body);

void physics_sync_gameobject_it(deccan_flecs_iter_t *it);
void physics_sync_physicsbody_it(deccan_flecs_iter_t *it);
