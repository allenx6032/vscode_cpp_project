#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

////////////////////////////////
//~ k: Build Options

#define BUILD_TITLE "Rubik"
#define OS_FEATURE_GRAPHICAL 1

// Third Party
#define CGLTF_IMPLEMENTATION
#include "cgltf.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
// #include "external/linmath.h"
#include "xxhash.c.h"
#include "xxhash.h"

// [h]
#include "rubik_base_inc.h"
#include "rubik_os_inc.h"
#include "rubik_render_inc.h"
#include "rubik_font_provider_inc.h"
#include "rubik_font_cache.h"
#include "rubik_draw.h"
#include "rubik_ui_inc.h"
#include "rubik_serialize_inc.h"
#include "rubik_physics_inc.h"
#include "rubik_core.h"
#include "rubik_ui_widget.h"
#include "rubik_asset.h"
#include "rubik_scenes_inc.h"

// [c]
#include "rubik_base_inc.c.h"
#include "rubik_os_inc.c.h"
#include "rubik_render_inc.c.h"
#include "rubik_font_provider_inc.c.h"
#include "rubik_font_cache.c.h"
#include "rubik_draw.c.h"
#include "rubik_ui_inc.c.h"
#include "rubik_serialize_inc.c.h"
#include "rubik_physics_inc.c.h"
#include "rubik_core.c.h"
#include "rubik_ui_widget.c.h"
#include "rubik_asset.c.h"
#include "rubik_scenes_inc.c.h"

internal void
entry_point(CmdLine *cmd_line)
{
  /////////////////////////////////
  //~ Init

  Vec2F32 default_resolution = {900, 900};
  String8 window_title = str8_lit("Rubik");

  //- Open window
  OS_Handle os_wnd = os_window_open(default_resolution, 0, window_title);
  os_window_first_paint(os_wnd);

  //- Render initialization
  r_init((char *)window_title.str, os_wnd, BUILD_DEBUG);
  R_Handle wnd = r_window_equip(os_wnd);

  Arena *frame_arena = arena_alloc();

  //- Time delta
  // TODO: using moving average to smooth out the value
  U64 frame_dt_us  = 0;
  U64 frame_us     = os_now_microseconds();
  U64 cpu_start_us = 0;
  U64 cpu_end_us   = 0;
  U64 cpu_dt_us    = 0;
  U64 gpu_start_us = 0;
  U64 gpu_end_us   = 0;
  U64 gpu_dt_us    = 0;

  //- User interaction
  OS_EventList events;

  //- Init ui state
  UI_State *ui = ui_state_alloc();
  ui_select_state(ui);

  //- Init game state
  rk_init(os_wnd);

  //- Load scene template and load default scene
  rk_state->template_count = ArrayCount(rk_scene_templates);
  rk_state->templates = rk_scene_templates;

  RK_Scene *default_scene = rk_state->templates[0].fn();
  rk_state->active_scene = default_scene;

  // Hot id
  U64 hot_id = 0;

  /////////////////////////////////
  // Main game loop

  while(rk_state->window_should_close == 0)
  {
    ProfTick();
    ProfBegin("frame");
    rk_state->debug.frame_dt_us = frame_dt_us;
    rk_state->debug.cpu_dt_us = cpu_dt_us;
    rk_state->debug.gpu_dt_us = gpu_dt_us;

    frame_dt_us = os_now_microseconds()-frame_us;
    frame_us = os_now_microseconds();

    //- k: begin of frame
    ProfBegin("frame begin");
    r_begin_frame();
    r_window_begin_frame(os_wnd, wnd);
    gpu_end_us = os_now_microseconds();
    gpu_dt_us = (gpu_end_us - gpu_start_us);
    cpu_start_us = os_now_microseconds();
    d_begin_frame();

    //- Poll events
    events = os_get_events(frame_arena, 0);

    ProfEnd();

    /////////////////////////////////
    //~ Game frame

    // TODO(k): calculation of frame_dt_us may be not accurate
    D_Bucket *d_bucket = rk_frame(events, frame_dt_us, hot_id);

    /////////////////////////////////
    //~ End of frame

    // TODO(XXX): the order here may be wrong, check later
    gpu_start_us = os_now_microseconds();
    d_submit_bucket(os_wnd, wnd, d_bucket);
    hot_id = r_window_end_frame(wnd, rk_state->cursor);
    r_end_frame();
    arena_clear(frame_arena);

    cpu_end_us = os_now_microseconds();
    cpu_dt_us = cpu_end_us - cpu_start_us;
    ProfEnd();
  }

  /////////////////////////////////
  //~ Cleanup

  os_window_close(os_wnd);
}
