#include <psim.h>


static bool on_frame(input_state_t *input_state, controlflow_t *cf, void *userdata) {
  game_state_t *game_state = userdata;

  if (input_state_quit(input_state)) {
    *cf = controlflow_exit;
    return false;
  }

  game_state_update(game_state);
  game_state_draw(game_state);

  return true;
}


int main(int argc, char *argv[]) {
  rng_init();

  game_state_t *game_state = game_state_create(800, 600);
  window_t *win = window_create("PSim", 800, 600);

  window_run_event_loop(win, on_frame, game_state);

  window_destroy(win);
  game_state_destroy(game_state);

  return 0;
}
