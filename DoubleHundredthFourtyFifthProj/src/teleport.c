#include "world.h"
#include "character.h"

void teleport_load() {
	DARNIT_FILE *f;
	void *data;
	unsigned int teleports;

	my_ws.char_data->teleport.entry = NULL;
	my_ws.char_data->teleport.entries = 0;

	if (!(f = d_file_open("world/teleports.dat", "rb"))) {
		fprintf(stderr, "Unable to load file world/teleports.dat\n");
		return;
	}

	d_file_read_ints(&teleports, 1, f);
	my_ws.char_data->teleport.entry = malloc(sizeof(*my_ws.char_data->teleport.entry) * teleports);
	my_ws.char_data->teleport.entries = teleports;
	d_file_read_ints(&teleports, 1, f);

	data = malloc(teleports);
	d_file_read(data, teleports, f);
	d_util_decompress(data, teleports, (void **) &my_ws.char_data->teleport.entry);
	free(data);
	d_util_endian_convert((void *) my_ws.char_data->teleport.entry, my_ws.char_data->teleport.entries);

	return;
}


void teleport_unload() {
	free(my_ws.char_data->teleport.entry);
	my_ws.char_data->teleport.entry = NULL;
	my_ws.char_data->teleport.entries = 0;

	return;
}
