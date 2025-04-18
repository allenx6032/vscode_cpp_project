#include <psim_game-state.h>
#include <psim_random.h>
#include <psim_helpers.h>
#include <math.h>


game_state_t *game_state_create(int screen_width, int screen_height) {
  game_state_t *game_state = calloc(1, sizeof(game_state_t));
  RUN_CHECK(game_state != NULL, "Could not allocate %llu bytes", sizeof(game_state_t));

  for (int particle_idx = 0; particle_idx < PARTICLE_COUNT; particle_idx++) {
    particle_t particle;

    if (particle_idx < PARTICLE_COUNT / 4) {
      particle.color = RED;
    }
    else if (particle_idx < PARTICLE_COUNT / 2) {
      particle.color = GREEN;
    }
    else if (particle_idx < 3 * PARTICLE_COUNT / 4) {
      particle.color = BLUE;
    }
    else {
      particle.color = WHITE;
    }

    particle.x = random_int(0, screen_width);
    particle.y = random_int(0, screen_height);
    particle.vx = 0;
    particle.vy = 0;

    game_state->particles[particle_idx] = particle;
  }

  game_state->interactions[particle_pair(RED, RED)]   = random_float(-1, 1);
  game_state->interactions[particle_pair(RED, GREEN)] = random_float(-1, 1);
  game_state->interactions[particle_pair(RED, BLUE)]  = random_float(-1, 1);
  game_state->interactions[particle_pair(RED, WHITE)] = random_float(-1, 1);

  game_state->interactions[particle_pair(GREEN, RED)]   = random_float(-1, 1);
  game_state->interactions[particle_pair(GREEN, GREEN)] = random_float(-1, 1);
  game_state->interactions[particle_pair(GREEN, BLUE)]  = random_float(-1, 1);
  game_state->interactions[particle_pair(GREEN, WHITE)] = random_float(-1, 1);

  game_state->interactions[particle_pair(BLUE, RED)]    = random_float(-1, 1);
  game_state->interactions[particle_pair(BLUE, GREEN)]  = random_float(-1, 1);
  game_state->interactions[particle_pair(BLUE, BLUE)]   = random_float(-1, 1);
  game_state->interactions[particle_pair(BLUE, WHITE)]  = random_float(-1, 1);

  game_state->interactions[particle_pair(WHITE, RED)]   = random_float(-1, 1);
  game_state->interactions[particle_pair(WHITE, GREEN)] = random_float(-1, 1);
  game_state->interactions[particle_pair(WHITE, BLUE)]  = random_float(-1, 1);
  game_state->interactions[particle_pair(WHITE, WHITE)] = random_float(-1, 1);

  return game_state;
}


void game_state_destroy(game_state_t *game_state) {
  DBG_CHECK(game_state != NULL, "NULL game state");

  free(game_state);
}



void game_state_update(game_state_t *game_state) {
  DBG_CHECK(game_state != NULL, "NULL game state");

  for (int a_particle_idx = 0; a_particle_idx < PARTICLE_COUNT; a_particle_idx++) {
    particle_t *a = &(game_state->particles[a_particle_idx]);
    float fx = 0, fy = 0;

    for (int b_particle_idx = 0; b_particle_idx < PARTICLE_COUNT; b_particle_idx++) {
      particle_t *b = &(game_state->particles[b_particle_idx]);

      float dx = (float)(a->x - b->x);
      float dy = (float)(a->y - b->y);
      float d = sqrtf(dx * dx + dy * dy);

      if (d > 0 && d < 80) {
        float g = game_state->interactions[particle_pair(a->color, b->color)];
        float force = g * 1 / d;
        fx += force * dx * (1.0 / 60);
        fy += force * dy * (1.0 / 60);
      }
    }

    a->vx += fx;
    a->vy += fy;

    a->x += a->vx;
    a->y += a->vy;

    if (a->x <= 0 || a->x >= 800) {
      a->vx *= -1;
    }
    if (a->y <= 0 || a->y >= 600) {
      a->vy *= -1;
    }
  }
}


void game_state_draw(game_state_t *game_state) {
  DBG_CHECK(game_state != NULL, "NULL game state");

  glClear(GL_COLOR_BUFFER_BIT);

  glPointSize(5);

  for (int particle_idx = 0; particle_idx < PARTICLE_COUNT; particle_idx++) {
    particle_t *particle = &(game_state->particles[particle_idx]);

    switch (particle->color) {
      case RED:    glColor3f(1, 0, 0); break;
      case GREEN:  glColor3f(0, 1, 0); break;
      case BLUE:   glColor3f(0, 0, 1); break;
      case WHITE:  glColor3f(1, 1, 1); break;
    }

    glBegin(GL_POINTS);
    glVertex2i(particle->x, particle->y);
    glEnd();
  }

  glFlush();
}
