/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/**
    @file yoyoengine.h
    @brief The master header for yoyoengine. Includes all other headers.
*/

#ifndef YE_ENGINE_MAIN_H
#define YE_ENGINE_MAIN_H
#define NK_INCLUDE_FIXED_TYPES

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

/*
    Include all our headers in one place.
    This lets the game use
    #include <yoyoengine.h> assuming they have
    defined the header and lib paths correctly.
*/
#include "yoyo_engine.h"         // init and core
#include "yoyo_config.h"         // jansson wrapper for config files
#include "yoyo_input.h"
#include "yoyo_yep.h"            // custom binary format parser/packer
#include "yoyo_json.h"           // jansson wrapper
#include "yoyo_graphics.h"
#include "yoyo_debug_renderer.h"
#include "uthash.h"
#include "yoyo_cache.h"
#include "yoyo_ui.h"
#include "yoyo_ecs.h"
#include "yoyo_button.h"
#include "yoyo_audiosource.h"
#include "yoyo_camera.h"
#include "yoyo_ecs.h"
#include "yoyo_physics.h"
#include "yoyo_renderer.h"
#include "yoyo_transform.h"
#include "yoyo_collider.h"
#include "yoyo_tag.h"
#include "lua_script.h"
#include "yoyo_utils.h"
#include "yoyo_timer.h"
#include "yoyo_audio.h"
#include "yoyo_logging.h"        // logging
#include "lua_api.h"        // scripting api
#include "yoyo_scene.h"          // scene manager
#include "yoyo_tricks.h"         // plugin system

#endif // YE_ENGINE_MAIN_H