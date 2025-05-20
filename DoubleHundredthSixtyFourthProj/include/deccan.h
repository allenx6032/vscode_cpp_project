/* Deccan Game Engine - C11 2D SDL Game Engine.
 * Copyright 2020, 2021 Ayush Bardhan Tripathy
 *
 * This software is licensed under MIT License.
 * See LICENSE.md included with this package for more info.
 */

#pragma once
#include "deccan_config.h"

#include "deccan_core.h"
#include "deccan_file_sys.h"
#include "deccan_utils.h"
#include "deccan_timer.h"
#include "deccan_maths.h"
#include "deccan_asset_manager.h"
#include "deccan_asset_descriptor.h"
#include "deccan_input.h"
#include "deccan_input_codes.h"

#include "deccan_physics.h"
#include "deccan_collision.h"

#include "deccan_color.h"
#include "deccan_generic_pipeline.h"
#include "deccan_renderer.h"
#include "deccan_render_types.h"
#include "deccan_texture.h"

#include "deccan_object.h"
#include "deccan_scene.h"
#include "deccan_scene_layer.h"
#include "deccan_world.h"
#include "deccan_camera.h"
#include "deccan_flecs.h"
#include "deccan_transform.h"
#include "deccan_drawable_geometry.h"
#include "deccan_drawable_sprite.h"
#include "deccan_rigid_body.h"
#include "deccan_collider.h"
#include "deccan_legacy_collider.h"

DE_API bool deccan_app_init(deccan_core_desc_t *settings);
DE_API void deccan_app_update(void);
DE_API void deccan_app_quit(void);
