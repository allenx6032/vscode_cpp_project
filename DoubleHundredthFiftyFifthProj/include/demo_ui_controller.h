
#ifndef UI_CONTROLLER
#define UI_CONTROLLER

#include <yoyoengine.h>
#include "demo_player_controller.h"

extern int heartrate;

void ui_controller_attatch();

char * keycode_to_label(SDL_Keycode code);

void ui_refresh_bind_labels(struct bind_state_data sd);

void ui_controller_update_bind_ui(struct bind_state_data * sd, float x, float y);

void ui_controller_additional_render();

void begin_ui_transform_blood();

void begin_dialog(char **tmp_dialog, int num_lines, void(*callback)());

void ui_progress_dialog();

void set_fonts_and_colors_dialog(const char *font, const char *color, int font_size);

void reset_fonts_and_colors_dialog();

#endif