#pragma once

#include <stdbool.h>

#include <SDL.h>


typedef struct input_state_s input_state_t;
struct input_state_s {
  bool quit;

  const Uint8 *kb_state_prev;
  const Uint8 *kb_state;
  Uint16 kb_mod_state;

  int mouse_x;
  int mouse_y;
  Uint32 mouse_state;

  int mouse_x_prev;
  int mouse_y_prev;
  Uint32 mouse_state_prev;
};


void input_state_init(input_state_t *input_state);
bool input_state_update(input_state_t *input_state, SDL_Event *evt);
void input_state_main_events_cleared(input_state_t *input_state);

bool input_state_quit(input_state_t *input_state);

bool input_state_keyboard_check_key(input_state_t *input_state, SDL_KeyCode key);
bool input_state_keyboard_check_key_pressed(input_state_t *input_state, SDL_KeyCode key);
bool input_state_keyboard_check_key_released(input_state_t *input_state, SDL_KeyCode key);
bool input_state_keyboard_check_modmask(input_state_t *input_state, Uint16 modmask);

bool input_state_mouse_check_button(input_state_t *input_state, Uint32 btnmask);
bool input_state_mouse_check_button_pressed(input_state_t *input_state, Uint32 btnmask);
bool input_state_mouse_check_button_released(input_state_t *input_state, Uint32 btnmask);
void input_state_mouse_get_relative(input_state_t *input_state, int *dx, int *dy);
