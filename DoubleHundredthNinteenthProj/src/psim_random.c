#include <psim_random.h>


void rng_init(void) {
  srand(time(NULL));
}


int random_int(int min, int max) {
  return (rand() % (max + 1) - min) + min;
}


float random_float(float min, float max) {
  return ((max - min) * ((float)rand() / RAND_MAX)) + min;
}
