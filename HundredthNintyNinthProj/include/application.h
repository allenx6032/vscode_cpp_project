#pragma once

#include "defines.h"
#include "platform.h"
#include "vertex_array.h"
#include "shader.h"
#include "texture2d.h"

#include "vector.h"

typedef void (*game_init_callback)();
typedef void (*game_update_callback)();
typedef void (*game_terminate_callback)();

typedef struct application_state {
    bool8 running;
    float delta_time;
    bool8 minimized;
} application_state;

typedef struct game_callbacks {
    game_init_callback game_init_cb;
    game_update_callback game_update_cb;
    game_terminate_callback game_terminate_cb;
} game_callbacks;

game_callbacks game_callbacks_create();

typedef struct application {
    application_state state;
    window* wnd;
    game_callbacks game_cbs;
} application;

application* application_create(window_properties props, game_callbacks game_cbs);

void application_run(application* app);

void application_shutdown(application* app);

void application_stop(application* app);