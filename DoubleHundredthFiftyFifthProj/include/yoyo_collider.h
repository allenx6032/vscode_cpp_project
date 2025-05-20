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
 * @file collider.h
 * @brief ECS Collider component\
 * 
 * TODO:
 * - the flexibility of having "round" colliders or triange colliders
 * - the ability to have multiple colliders on a single entity
 * - on enter, on exit on stay
 * - trigger colliders
 */

#ifndef YE_COLLIDER_H
#define YE_COLLIDER_H

#include <stdbool.h>
#include <yoyo_ecs.h>
#include <yoyo_utils.h>

/**
 * @brief A structure representing a collider component.
 *
 * This structure might need to be broken out into two different colliders, trigger and static to enable
 * multiple colliders on a single entity. Also, consider giving this a z or a layer, colliders do not care about the layer right now.
 */
struct ye_component_collider {
    bool active;            /**< Controls whether system will act upon this component. */
    
    bool relative;          /**< Specifies whether this component is relative to a parent transform. */
    
    struct ye_rectf rect;   /**< The collider rectangle. */

    bool is_trigger;        /**< Specifies whether this collider is a trigger. If false, it is a static collider. */

    /*
        Meta tracking trigger states
    */
    // bool _trigger_entered;
};

/**
 * @brief Adds a static collider to an entity.
 *
 * @param entity The entity to which the collider is to be added.
 * @param rect The rectangle defining the collider.
 */
void ye_add_static_collider_component(struct ye_entity *entity, struct ye_rectf rect);

/**
 * @brief Adds a trigger collider to an entity.
 *
 * @param entity The entity to which the collider is to be added.
 * @param rect The rectangle defining the collider.
 */
void ye_add_trigger_collider_component(struct ye_entity *entity, struct ye_rectf rect);

/**
 * @brief Removes an entity's collider component.
 *
 * @param entity The entity from which the collider is to be removed.
 */
void ye_remove_collider_component(struct ye_entity *entity);

#endif