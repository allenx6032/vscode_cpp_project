#include <psim_input.h>
#include <psim_helpers.h>


void input_state_init(input_state_t *input_state) {
  DBG_CHECK(input_state != NULL, "NULL input state");

  input_state->quit = false;

  input_state->kb_state_prev = SDL_GetKeyboardState(NULL);
  input_state->kb_state = NULL;
  input_state->kb_mod_state = KMOD_NONE;

  input_state->mouse_x = 0;
  input_state->mouse_y = 0;
  input_state->mouse_state = 0;
  input_state->mouse_state_prev = SDL_GetMouseState(
    &(input_state->mouse_x_prev),
    &(input_state->mouse_y_prev)
  );
}


bool input_state_update(input_state_t *input_state, SDL_Event *evt) {
  DBG_CHECK(input_state != NULL, "NULL input state");
  DBG_CHECK(evt != NULL, "NULL event");

  bool redraw_requested = false;

  switch (evt->type) {
    case SDL_QUIT:
      input_state->quit = true;
      break;

    case SDL_WINDOWEVENT:
      switch (evt->window.event) {
        case SDL_WINDOWEVENT_SHOWN:
        case SDL_WINDOWEVENT_EXPOSED:
        case SDL_WINDOWEVENT_RESIZED:
        case SDL_WINDOWEVENT_SIZE_CHANGED:
        case SDL_WINDOWEVENT_MAXIMIZED:
        case SDL_WINDOWEVENT_RESTORED:
        case SDL_WINDOWEVENT_ENTER:
        case SDL_WINDOWEVENT_FOCUS_GAINED:
        case SDL_WINDOWEVENT_TAKE_FOCUS:
          redraw_requested = true;
          break;
      }
      break;
  }

  return redraw_requested;
}


void input_state_main_events_cleared(input_state_t *input_state) {
  DBG_CHECK(input_state != NULL, "NULL input state");

  input_state->kb_state = SDL_GetKeyboardState(NULL);
  input_state->kb_mod_state = SDL_GetModState();

  input_state->mouse_state = SDL_GetMouseState(
    &(input_state->mouse_x),
    &(input_state->mouse_y)
  );
}


bool input_state_quit(input_state_t *input_state) {
  DBG_CHECK(input_state != NULL, "NULL input state");

  return input_state->quit;
}


bool input_state_keyboard_check_key(input_state_t *input_state, SDL_KeyCode key) {
  DBG_CHECK(input_state != NULL, "NULL input state");

  SDL_Scancode scancode = SDL_GetScancodeFromKey(key);
  return input_state->kb_state[scancode] == 1;
}


bool input_state_keyboard_check_key_pressed(input_state_t *input_state, SDL_KeyCode key) {
  DBG_CHECK(input_state != NULL, "NULL input state");

  SDL_Scancode scancode = SDL_GetScancodeFromKey(key);
  return (
    input_state->kb_state[scancode] == 1
    && input_state->kb_state_prev[scancode] == 0
  );
}


bool input_state_keyboard_check_key_released(input_state_t *input_state, SDL_KeyCode key) {
  DBG_CHECK(input_state != NULL, "NULL input state");

  SDL_Scancode scancode = SDL_GetScancodeFromKey(key);
  return (
    input_state->kb_state[scancode] == 0
    && input_state->kb_state_prev[scancode] == 1
  );
}


bool input_state_keyboard_check_modmask(input_state_t *input_state, Uint16 modmask) {
  DBG_CHECK(input_state != NULL, "NULL input state");

  return (input_state->kb_mod_state & modmask) != 0;
}


bool input_state_mouse_check_button(input_state_t *input_state, Uint32 btnmask) {
  DBG_CHECK(input_state != NULL, "NULL input state");

  return (input_state->mouse_state & btnmask) != 0;
}


bool input_state_mouse_check_button_pressed(input_state_t *input_state, Uint32 btnmask) {
  DBG_CHECK(input_state != NULL, "NULL input state");

  return (
    (input_state->mouse_state & btnmask) != 0
    && (input_state->mouse_state_prev & btnmask) == 0
  );
}


bool input_state_mouse_check_button_released(input_state_t *input_state, Uint32 btnmask) {
  DBG_CHECK(input_state != NULL, "NULL input state");

  return (
    (input_state->mouse_state & btnmask) == 0
    && (input_state->mouse_state_prev & btnmask) != 0
  );
}


void input_state_mouse_get_relative(input_state_t *input_state, int *dx, int *dy) {
  DBG_CHECK(input_state != NULL, "NULL input state");

  if (dx != NULL) {
    *dx = input_state->mouse_x - input_state->mouse_x_prev;
  }

  if (dy != NULL) {
    *dy = input_state->mouse_y - input_state->mouse_y_prev;
  }
}
