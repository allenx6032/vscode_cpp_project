#include <psim_window.h>
#include <psim_helpers.h>


window_t *window_create(const char *title, int width, int height) {
  window_t *win = malloc(sizeof(window_t));
  RUN_CHECK(win != NULL, "Could not allocate %llu bytes", sizeof(window_t));

  int res = SDL_Init(SDL_INIT_VIDEO);
  RUN_CHECK(res >= 0, "Unable to initialize SDL");

  win->screen = SDL_CreateWindow(
    title,
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    width, height,
    SDL_WINDOW_OPENGL
  );
  RUN_CHECK(win->screen != NULL, "Unable to initialize window");

  win->gl_context = SDL_GL_CreateContext(win->screen);
  RUN_CHECK(win->gl_context != NULL, "Unable to initialize OpenGL context");

  glClearColor(0, 0, 0.1, 0);
  gluOrtho2D(0, width, height, 0);

  return win;
}


void window_destroy(window_t *win) {
  DBG_CHECK(win != NULL, "NULL window");

  SDL_GL_DeleteContext(win->gl_context);
  SDL_DestroyWindow(win->screen);
  SDL_Quit();

  free(win);
}


static bool window_handle_events(input_state_t *input_state) {
  SDL_Event evt;
  bool redraw_request = false;

  input_state_init(input_state);

  while (SDL_PollEvent(&evt) > 0) {
    redraw_request |= input_state_update(input_state, &evt);
  }

  input_state_main_events_cleared(input_state);

  return redraw_request;
}


void window_run_event_loop(window_t *win, window_frame_cb cb, void *userdata) {
  DBG_CHECK(win != NULL, "NULL window");
  DBG_CHECK(cb != NULL, "NULL callback");

  controlflow_t cf = controlflow_poll;

  const int fps = 60;
  const int frame_delay = 1000 / fps;

  while (cf != controlflow_exit) {
    Uint32 frame_start = SDL_GetTicks();

    input_state_t input_state;
    bool redraw_request = false;

    redraw_request |= window_handle_events(&input_state);
    redraw_request |= cb(&input_state, &cf, userdata);

    if (redraw_request) {
      SDL_GL_SwapWindow(win->screen);
    }

    Uint32 frame_duration = SDL_GetTicks() - frame_start;
    if (frame_delay > frame_duration) {
      SDL_Delay(frame_delay - frame_duration);
    }
  }
}
