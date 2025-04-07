#pragma once

#include <vector3.h>
#include <vector2.h>
#include <matrix4.h>

typedef struct player_camera {
    matrix4 view_matrix;
    vector2 position;
} player_camera;

player_camera player_camera_create(vector2 position);

void player_camera_update(player_camera* camera);