#pragma once

#include <stdbool.h>

#include <SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <psim_input.h>


typedef struct window_s window_t;
struct window_s {
  SDL_Window *screen;
  SDL_GLContext gl_context;
};


typedef enum controlflow_e controlflow_t;
enum controlflow_e {
  controlflow_exit = 0,
  controlflow_poll = 1,
};

typedef bool (*window_frame_cb)(input_state_t*, controlflow_t *, void *);


window_t *window_create(const char *title, int width, int height);
void window_destroy(window_t *win);

void window_run_event_loop(window_t *win, window_frame_cb cb, void *userdata);
