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

#include <stdbool.h>

#include <yoyo_engine.h>
#include <yoyo_camera.h>

void ye_set_camera(struct ye_entity *entity){
    YE_STATE.engine.target_camera = entity;
}

void ye_add_camera_component(struct ye_entity *entity, int z, SDL_Rect view_field){
    entity->camera = malloc(sizeof(struct ye_component_camera));
    entity->camera->active = true;
    entity->camera->view_field = view_field; // x and y are used as an offset from the transform on the camera
    entity->camera->z = z;
    entity->camera->relative = true;

    // log that we added a transform and to what ID
    // ye_logf(debug, "Added camera to entity %d\n", entity->id);

    // add this entity to the camera component list
    ye_entity_list_add(&camera_list_head, entity);
}

void ye_remove_camera_component(struct ye_entity *entity){
    free(entity->camera);
    entity->camera = NULL;

    // remove the entity from the camera component list
    ye_entity_list_remove(&camera_list_head, entity);
}