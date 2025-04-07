#include "player_camera.h"

#include <global_state.h>
#include <matrix_transform.h>
#include <platform.h>
#include <event_system.h>
#include <batch_renderer.h>

player_camera player_camera_create(vector2 position) {
    player_camera ret;
    ret.position = position;
    ret.view_matrix = matrix4_identity();
    return ret;
}

void player_camera_update(player_camera* camera) {
    if (platform_is_key_down(KEY_W)) {
        camera->position.y += 130 * g_state->app->state.delta_time;
    }
    if (platform_is_key_down(KEY_S)) {
        camera->position.y -= 130 * g_state->app->state.delta_time;
    }
    if (platform_is_key_down(KEY_D)) {
        camera->position.x += 130 * g_state->app->state.delta_time;
    }
    if (platform_is_key_down(KEY_A)) {
        camera->position.x -= 130 * g_state->app->state.delta_time;
    }
    matrix4 transform = translate_mv(matrix4_identity(),
                                     vector3_create(camera->position.x, camera->position.y, 0.0f));
    camera->view_matrix = matrix4_inverse(transform);
}