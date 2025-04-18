#pragma once

#include <psim_engine.h>


#define PARTICLE_COUNT    1000


typedef enum particle_color_e particle_color_t;
enum particle_color_e {
  RED,
  GREEN,
  BLUE,
  WHITE,
};

typedef struct particle_s particle_t;
struct particle_s {
  particle_color_t color;

  int x, y;
  float vx, vy;
};

typedef struct game_state_s game_state_t;
struct game_state_s {
  particle_t particles[PARTICLE_COUNT];
  float interactions[16];
};


#define particle_pair(a,b)    (((int)b) + ((int)a) * 4)


game_state_t *game_state_create(int screen_width, int screen_height);
void game_state_destroy(game_state_t *game_state);

void game_state_update(game_state_t *game_state);
void game_state_draw(game_state_t *game_state);
