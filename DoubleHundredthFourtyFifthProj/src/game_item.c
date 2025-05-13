#include "savefile.h"
#include "character.h"
#include "game_item.h"
#include "world.h"
#include "item_reply.h"
#include <stdlib.h>
#include <string.h>
#include <darnit_api.h>


void item_init(const char *item_table) {
	struct item item;
	DARNIT_FILE *f;
	char buff[512], namebuff[64], descbuff[384], handler[32];
	int dataval, stack;
	
	my_ws.item.type = NULL;
	my_ws.item.types = 0;
	
	if (!(f = d_file_open(item_table, "rb")))
		return;
	item.type = NULL;
	item.types = 0;

	/* TODO: Read item table */
	while (!d_file_eof(f)) {
		d_file_gets(buff, 512, f);
		*namebuff = *descbuff = *handler = 0;
		sscanf(buff, "%[^\t] %[^\t] %s %i %i", namebuff, descbuff, handler, &dataval, &stack);
		if (!*handler)
			continue;
		item.type = realloc(item.type, sizeof(*item.type) * (item.types + 1));
		item.type[item.types].name = strdup(namebuff);
		item.type[item.types].description = strdup(descbuff);
		item.type[item.types].datavalue = dataval;
		item.type[item.types].max_stack = stack;
		item.type[item.types].handler = character_find_ai_func(handler);
		item.types++;
	}
	

	d_file_close(f);
	my_ws.item = item;
	
	return;
}


void item_destroy() {
	int i;

	for (i = 0; i < my_ws.item.types; i++)
		free(my_ws.item.type[i].description), free(my_ws.item.type[i].name);
	free(my_ws.item.type);
	my_ws.item.type = NULL;
	my_ws.item.types = 0;

	return;
}


struct inventory *inventory_new(int size) {
	struct inventory *inv;
	int i;

	inv = malloc(sizeof(*inv));
	inv->entry = malloc(sizeof(*inv->entry) * size);
	for (i = 0; i < size; i++)
		inv->entry[i].type = inv->entry[i].amount = -1;
	return inv;
}


struct inventory *inventory_destroy(struct inventory *inv) {
	free(inv->entry);
	free(inv);

	return NULL;
}


void item_use(struct inventory *inv, int item, int char_src, int char_dst) {
	struct item_reply ir;
	int ii, i;

	if (my_ws.item.types <= inv->entry[item].type)
		return;
	ii = inv->entry[item].type;
	if (!my_ws.item.type[ii].handler)
		return;
	ir = my_ws.item.type[ii].handler(my_ws.item.type[ii].datavalue, my_ws.char_data->entry[char_src], my_ws.char_data->entry[char_dst]);


	/* Source */
	ii = my_ws.char_data->entry[char_src]->item_replies;
	my_ws.char_data->entry[char_src]->item_reply = realloc(my_ws.char_data->entry[char_src]->item_reply, ii + ir.srcs);
	for (i = 0; i < ir.srcs; i++)
		my_ws.char_data->entry[char_src]->item_reply[i + ii] = ir.src[i];
	my_ws.char_data->entry[char_src]->item_replies += ir.srcs;

	/* Destination */
	ii = my_ws.char_data->entry[char_dst]->item_replies;
	my_ws.char_data->entry[char_dst]->item_reply = realloc(my_ws.char_data->entry[char_dst]->item_reply, ii + ir.srcs);
	for (i = 0; i < ir.dsts; i++)
		my_ws.char_data->entry[char_dst]->item_reply[i + ii] = ir.dst[i];
	my_ws.char_data->entry[char_dst]->item_replies += ir.dsts;
		

	/* Just testing */
	fprintf(stderr, "*** Source diff ***\n");
	for (i = 0; i < ir.srcs; i++)
		fprintf(stderr, "%s: %i\n", ir.src[i].stat, ir.src[i].diff);
	free(ir.src);
	fprintf(stderr, "*** Destination diff ***\n");
	for (i = 0; i < ir.srcs; i++)
		fprintf(stderr, "%s: %i\n", ir.dst[i].stat, ir.dst[i].diff);
	free(ir.dst);

	return;
}

