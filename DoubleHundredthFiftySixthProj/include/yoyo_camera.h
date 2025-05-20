/*
    This file is a part of yoyoengine. (https://github.com/yoyolick/yoyoengine)
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
 * @file camera.h
 * @brief ECS Camera component
 */

#ifndef YE_CAMERA_H
#define YE_CAMERA_H

#include <stdbool.h>
#include <yoyo_ecs.h>
#include <SDL.h>

/**
 * @brief The camera component
 * 
 * Holds some information about a camera and its view field (the x and y of the view field are unused, as its location is inferred from its transform)
*/
struct ye_component_camera {
    bool active;    // controls whether system will act upon this component

    bool relative;  // whether or not this comp is relative to a parent transform
    int z; // the layer the camera sits on

    SDL_Rect view_field;    // view field of camera
};

/**
 * @brief Sets the camera to be used by the renderer
 * 
 * @param entity The entity to set as the camera
 */
void ye_set_camera(struct ye_entity *entity);

/**
 * @brief Adds a camera component to an entity
 * 
 * @param entity The entity to add the component to
 * @param z The layer the camera sits on
 * @param view_field The view field of the camera
 */
void ye_add_camera_component(struct ye_entity *entity, int z, SDL_Rect view_field);

/**
 * @brief Removes a camera component from an entity
 * 
 * @param entity The entity to remove the component from
 */
void ye_remove_camera_component(struct ye_entity *entity);

#endif