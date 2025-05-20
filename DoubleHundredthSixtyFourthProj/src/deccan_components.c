/* Deccan Game Engine - C11 2D SDL Game Engine.
 * Copyright 2020, 2021 Ayush Bardhan Tripathy
 *
 * This software is licensed under MIT License.
 * See LICENSE.md included with this package for more info.
 */

#include <deccan_transform.h>
#include <deccan_drawable_geometry.h>
#include <deccan_drawable_sprite.h>
#include <deccan_rigid_body.h>
#include <deccan_collider.h>
#include <deccan_legacy_collider.h>
#include "deccan_components.h"

////////////////////////////////////////////////////////////////////////////////
// Registrar
////////////////////////////////////////////////////////////////////////////////

void components_register_all(void) {
    deccan_flecs_register_component("Transform", sizeof(deccan_comp_transform_t), ECS_ALIGNOF(deccan_comp_transform_t));

    deccan_flecs_register_component("Collider", sizeof(deccan_comp_collider_t), ECS_ALIGNOF(deccan_comp_collider_t));

    deccan_flecs_register_component("DrawableGeometry", sizeof(deccan_comp_drawable_geometry_t),
        ECS_ALIGNOF(deccan_comp_drawable_geometry_t));

    deccan_flecs_register_component("DrawableSprite", sizeof(deccan_comp_drawable_sprite_t),
        ECS_ALIGNOF(deccan_comp_drawable_sprite_t));

    deccan_flecs_register_component("RigidBody", sizeof(deccan_comp_rigid_body_t),
        ECS_ALIGNOF(deccan_comp_rigid_body_t));

    deccan_flecs_register_component("BoxCollider", sizeof(deccan_comp_box_collider_t),
        ECS_ALIGNOF(deccan_comp_box_collider_t));
}
