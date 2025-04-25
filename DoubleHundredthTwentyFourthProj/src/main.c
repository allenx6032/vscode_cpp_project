
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <glad.h>
#include <swfw.h>

struct swfw_context swfw_ctx = {0};
struct swfw_window swfw_win = {0};

#define APPLICATION_VERSION_YYYY 2019
#define APPLICATION_VERSION_MM 01
#define APPLICATION_VERSION_DD 24
#define APPLICATION_VERSION_MICRO 0
#define PATH_LENGTH_MAX 4096


struct context {
	uint8_t print_help;
	uint8_t file_output_path[PATH_LENGTH_MAX];
	FILE *file_output;
	uint32_t randomness_seed;
	uint32_t total_of_sequences;
	uint32_t sequence_size;
	uint8_t *units_distribution;
	uint32_t total_of_units;
	uint8_t show_progress;
};

static uint32_t rotr32(uint32_t x, uint32_t r)
{
	return x >> r | x << (-r & 31);
}

#define RND_MULTIPLIER 6364136223846793005u
#define RND_INCREMENT 1442695040888963407u

static uint32_t rnd_uint32(uint64_t *state)
{
	uint64_t x;
	uint32_t count;
	x = state[0];
	count = (uint32_t)(x >> 59);
	state[0] = x * (uint64_t)RND_MULTIPLIER + (uint64_t)RND_INCREMENT;
	x = x ^ (x >> 18);
	return rotr32((uint32_t)(x >> 27), count);
}

static void write_output(struct context *context)
{
	uint32_t total_of_sequences;
	uint8_t header[21];
	uint64_t randomness_state;
	total_of_sequences = 0;
	randomness_state = context->randomness_seed;
	while (total_of_sequences < context->total_of_sequences) {
		uint32_t sequence_size;
		memset(header, 0, 21);
		/* The index of the output is 1-based */
		snprintf((char *)header, 20, "%u", total_of_sequences + 1);
		fprintf(context->file_output, ">%s\n", header);
		sequence_size = 0;
		while (sequence_size < context->sequence_size) {
			uint8_t character;
			character = context->units_distribution[rnd_uint32(&randomness_state) % context->total_of_units];
			fputc(character, context->file_output);
			sequence_size = sequence_size + 1;
		}
		fputc('\n', context->file_output);
		total_of_sequences = total_of_sequences + 1;
		if (context->show_progress == 1 || total_of_sequences == context->total_of_sequences) {
			fprintf(stdout, "\rWriting sequences at %0.2f%% (sequence %u of %u).",
				((double)total_of_sequences / (double)context->total_of_sequences) * 100.0,
				total_of_sequences,
				context->total_of_sequences);
			fflush(stdout);
		}
	}
	if (context->show_progress == 1) {
		fprintf(stdout, "\n");
		fflush(stdout);
	}
}

static void print_help(void)
{
	printf("randomfasta (Random FASTA generator) version %04u.%02u.%02u.%u.\n",
		APPLICATION_VERSION_YYYY,
		APPLICATION_VERSION_MM,
		APPLICATION_VERSION_DD,
		APPLICATION_VERSION_MICRO);
	puts("Created by Felipe Ferreira da Silva.");
	puts("");
	puts("Usage:");
	puts("  randomfasta [1] [2] [3] [4] [5] [6]");
	puts("");
	puts("  1: Path to output file.");
	puts("  2: Total of sequences (must be at least 1).");
	puts("  3: Length of the sequences (must be at least 1).");
	puts("  4: Randomness seed (must be at least 1).");
	puts("  5: Units distribution (must contain at least 1 character).");
	puts("  6: Show progress (0 for no, 1 for yes).");
}

int main(int argc, char **args)
{
	enum swfw_status status = SWFW_OK;
	struct swfw_event event = {0};
	bool running = true;
	int32_t screen_width = 0;
	int32_t screen_height = 0;
	status = swfw_make_context(&swfw_ctx, SWFW_BACKEND_AUTOMATIC);
	if (status != SWFW_OK) {
		printf("Error: failed to create context.\n");
		abort();
	}
	if (swfw_get_screen_size(&swfw_ctx, 0, &screen_width, &screen_height) == SWFW_OK) {
		printf("Screen size: %d, %d.\n", screen_width, screen_height);
	}
	status = swfw_make_window(&swfw_ctx, &swfw_win);
	if (status != SWFW_OK) {
		printf("Error: failed to create window.\n");
		abort();
	}
	swfw_window_swap_interval(&swfw_win, 1);
	swfw_set_window_resizable(&swfw_win, false);
	swfw_show_window(&swfw_win);
	swfw_set_window_title(&swfw_win, "Simple");
	while (running) {
		while (swfw_poll_event(&swfw_ctx, &event)) {
			if (event.type == SWFW_EVENT_KEY_PRESS) {
				if (event.key_code) {
					printf("Event: key %d pressed.\n", event.key_code);
				}
				if (event.string_length) {
					printf("       utf %.*s pressed.\n", event.string_length, event.string);
				}
			} else if (event.type == SWFW_EVENT_KEY_RELEASE) {
				printf("Event: key %d released.\n", event.key_code);
			} else if (event.type == SWFW_EVENT_BUTTON_PRESS) {
				printf("Event: button pressed.\n");
				swfw_drag_window(&swfw_win);
			} else if (event.type == SWFW_EVENT_BUTTON_RELEASE) {
				printf("Event: button released.\n");
			} else if (event.type == SWFW_EVENT_CURSOR_MOTION) {
				printf("Event: cursor motion (%.0f, %.0f).\n", event.x, event.y);
			} else if (event.type == SWFW_EVENT_SCROLL) {
				printf("Event: scroll %u, %.2f.\n", event.axis, event.scroll);
			} else if (event.type == SWFW_EVENT_CURSOR_ENTER) {
				printf("Event: cursor enter.\n");
			} else if (event.type == SWFW_EVENT_CURSOR_LEAVE) {
				printf("Event: cursor leave.\n");
			} else if (event.type == SWFW_EVENT_DESTROY) {
				printf("Event: destroy window.\n");
				running = false;
			}
		}
		glClearColor(0.5, 0.5, 0.5, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		glFlush();
		swfw_window_swap_buffers(&swfw_win);
	}
	swfw_destroy_window(&swfw_win);
	swfw_destroy_context(&swfw_ctx);
	
	uint8_t success;
	//int32_t status;
	struct context *context;
	success = 1;
	context = malloc(sizeof(struct context));
	if (context == NULL) {
		success = 0;
		goto done;
	}
	memset(context, 0, sizeof(struct context));
	/* Arguments */
	if (argc == 7) {
		strncat((char *)context->file_output_path, args[1], PATH_LENGTH_MAX - 1);
		context->total_of_sequences = atoi(args[2]);
		context->sequence_size = atoi(args[3]);
		context->randomness_seed = atoi(args[4]);
		context->total_of_units = strlen(args[5]);
		if (context->total_of_units > 0) {
			context->units_distribution = malloc(context->total_of_units * sizeof(uint8_t) + 1);
			memset(context->units_distribution, 0, context->total_of_units * sizeof(uint8_t) + 1);
			memcpy(context->units_distribution, args[5], context->total_of_units);
		}
		context->show_progress = atoi(args[6]);
	} else {
		context->print_help = 1;
	}
	if (context->print_help == 1) {
		print_help();
		goto freecontext;
	}
	printf("  Output: \"%s\"\n", context->file_output_path);
	printf("  Total of sequences: %u\n", context->total_of_sequences);
	printf("  Sequence length: %u\n", context->sequence_size);
	printf("  Randomness seed: %u\n", context->randomness_seed);
	printf("  Units distribution: \"%s\"\n", (char *)context->units_distribution);
	context->file_output = fopen((char *)context->file_output_path, "w");
	if (context->file_output == NULL) {
		puts("Failed to open the output file for writing.");
		success = 0;
		goto freecontext;
	}
	if (context->total_of_sequences == 0) {
		puts("There must be at least 1 sequence.");
		success = 0;
		goto freecontext;
	}
	if (context->sequence_size == 0) {
		puts("The length of the sequence must be at least 1.");
		success = 0;
		goto freecontext;
	}
	if (context->total_of_units == 0) {
		puts("There must be at least 1 unit for the distribution.");
		success = 0;
		goto freecontext;
	}
	write_output(context);
freecontext:
	if (context->file_output != NULL) {
		fclose(context->file_output);
	}
	if (context->units_distribution != NULL) {
		free(context->units_distribution);
	}
	free(context);
done:
	if (success == 1) {
		status = EXIT_SUCCESS;
	} else {
		status = EXIT_FAILURE;
	}
	return status;
	// return EXIT_SUCCESS;
}
