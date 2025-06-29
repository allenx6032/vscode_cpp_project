
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <swfw.h>
#include <glad.h>

struct swfw_context swfw_ctx = {0};
struct swfw_window swfw_win = {0};

int32_t main(void)
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
	return EXIT_SUCCESS;
}
