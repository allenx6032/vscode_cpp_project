#include <raylib.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h" // Required for GUI controls

// custom header files
#include "graph_structures.h"
#include "graph_ui.h"
#include "graph.h"

// initialize array of euqations
Equation equation_arr[8];

char* vars[] = { "a:", "b:", "c:", "d:" };
 
 // Input Sliders
void draw_sliders();


int main(void)
{
	// init window
	init_window();

	// Main program loop
	while (!WindowShouldClose()) // Detect window close button or ESC key
	{
		BeginDrawing();
		ClearBackground(RAYWHITE);

		// draws grid
		draw_axis();
		// initialize euqations box num_eq times
		draw_graphs();
		// Draws sections of main window
		draw_sections();
		// Draws equations box
		draw_boxes();
		
		// Input sliders
		draw_sliders();

		//custom functions to draw window
		window_add();

		// screenshot and clear buttons
		draw_buttons();

		EndDrawing();
	}

	clean_up(); // Close window

	return 0;
}


void draw_sliders(){
	for (int i = 0; i < num_eq; i++) {
				GuiGroupBox((Rectangle) { 15, 55 + 120 * i, 270, 60 }, "Variables");
				for (int j = 0; j < equation_arr[i].type.var_num; j++) {
					float xcor = (j % 2 == 0) ? 30 : 160;
					float ycor = 120 * i + ((j / 2 != 0) ? 25 : 0);
					if (equation_arr[i].type.var_num <= 2)
						ycor += 75;
					else
						ycor += 65;
					DrawText(vars[j], xcor, ycor, 20, GRAY);
					equation_arr[i].type.value[j] = GuiSlider((Rectangle) { xcor + 40, ycor, 60, 20 }, "-10", "10", equation_arr[i].type.value[j], -10, 10);
			}
		}
}

