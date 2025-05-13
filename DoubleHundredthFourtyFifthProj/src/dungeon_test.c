#include <darnit_api.h>
#include <string.h>
#include <time.h>

#include "generate_world.h"
#include "random.h"
#include "save_loader.h"
#include "world.h"
#include "character.h"

int main(int argc, char **argv) {
	d_init("dungeon_gen", "dungeon_gen", NULL);
	random_seed(time(NULL));

	if (argc > 1)
		if (!strcmp(argv[1], "new"))
			generate_world(0);

	/* TODO: Move this into a separate function loading a world */
	world_init();
	
	world_load(0);
	
	my_ws.new_state = WORLD_STATE_MAPSTATE;
	world_loop();

	for (;;) {
		world_loop();
		d_loop();
	}

	d_quit();
	return 0;
}
