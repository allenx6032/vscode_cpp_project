#include <raylib.h>
#include <string.h>
#include <time.h>

// TODO: SlideListView
// custom header
#include "graph_ui.h"
#include "graph.h"
#include "graph_structures.h"
#include "raygui.h" // Required for GUI controls

Window windows[4] = {
	{"Polynomial", 5, {
					{"linear", 2, {0}},
					{"quadratic_x", 3, {0}},
					{ "quadratic_y",3, {0}},
					{"cubic_x",4, {0}},
					{ "cubic_y",4, {0}}
				}
		},
	{"Conic Section", 4, {
					{"ellipse",2, {0}},
					{"parabola",1, {0}},
					{"hyperbola",2, {0}},
					{"circle",1, {0}}
				}
		},
	{"Trigonometric", 3, {
					{"sin",3, {0}},
					{"cos",3, {0}},
					{"tan",3, {0}}
				}
		},
	{"Exponential", 3, {
					{"e_ax",1, {0}},
					{"a_x",1, {0}},
					{"log_a_x",1, {0}}
				}
		}
};

//arrays for list view
char* window_category[4] = { "Polynomial", "Conic Section", "Trigonometric", "Exponential" };
char* window_type[4][5] = {
	{"Linear", "Quadratic in X", "Quadratic in Y", "Cubic in X", "Cubic in Y"},
	{"Ellipse", "Parabola", "Hyberbola", "Circle"},
	{"Sin", "Cos", "Tan"},
	{"e^(ax)", "a^x", "log(base a) x"}
};


// Function initialization
char *labelBuilder(char *label, float a, float b, float c, float d);


// no of equations
int num_eq = 0;
bool window_Active = false;
bool added = false;

//for list view
int current_category = 0;
int current_type = 0;

bool editMode = true;
Font font_label; 
Font font_main;
Sound fx;

int init_window()
{
	const int screenWidth = 900;
	const int screenHeight = 600;
	InitWindow(screenWidth, screenHeight, "Grapher-C");
	InitAudioDevice();
	
	Image icon = LoadImage("res/icon.png");
	SetWindowIcon(icon);
	SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor())); // Set our game to run at max refresh rate of monitor
	font_main = LoadFontEx("res/code.otf", 15, 0, 250);
	font_label = LoadFontEx("res/cmunssdc.ttf", 30, 0, 250);
	GuiSetFont(font_main);
}

int draw_sections()
{
	DrawLine(GetScreenWidth() - GetScreenHeight(), 0, GetScreenWidth() - GetScreenHeight(), GetScreenHeight(), Fade(LIGHTGRAY, 0.6f));
	DrawRectangle(0, 0, GetScreenWidth() - 600, GetScreenHeight(), Fade(LIGHTGRAY, 0.3f));
	DrawRectangle(300, 550, 100, 50 , LIGHTGRAY);
	
}

int draw_buttons() {
	if (GuiButton((Rectangle) { 310, 560, 30, 30 }, "#113#")) {
		for (int i = 0; i < 5; i++) {
			//equation_arr[i] = {{"\0", 0, {0}}, false, BLANK};
			num_eq = 0;
		}
	}
	if (GuiButton((Rectangle) { 360, 560, 30, 30 }, "#104#")) {
		time_t t;
		time(&t);
		int ti = t;
		/* system("mkdir screenshots"); */
		TakeScreenshot("screenshots/tmp.png");
		Image uncropped = LoadImage("screenshots/tmp.png");
		ImageCrop(&uncropped, (Rectangle) {300,0,600,600});
		ExportImage(uncropped, TextFormat("screenshots/Grapher_%d.png", ti));
		fx = LoadSound("res/screen.wav");
		/* system("cd screenshots && del tmp.png"); */
		PlaySound(fx);
	}
}
int draw_boxes() {
	for (int i = 0; i < num_eq; i++)
		box_eq(i);
}

int draw_graphs() {
	for (int i = 0; i < num_eq; i++)
		draw_graph(i);
}


int window_add()
{
	if (num_eq < 5 && GuiButton((Rectangle) { 15, num_eq * 120 + 10, 270, 50 }, "Add Equation"))
	{
		window_Active = true;
		//num_eq++;
	}

	if (window_Active)
	{
		//add data

		window_Active = !GuiWindowBox((Rectangle) { 285, 45, 400, 310 }, "Add Equation");
		GuiGroupBox((Rectangle) { 290, 80, 190, 265 }, "Category");
		current_category = GuiListViewEx((Rectangle) { 295, 90, 180, 250 }, window_category, 4, 0, NULL, current_category);
		GuiGroupBox((Rectangle) { 490, 80, 190, 215 }, "Type");
		current_type = GuiListViewEx((Rectangle) { 495, 90, 180, 200 }, window_type[current_category], windows[current_category].typelen, 0, NULL, current_type);
		added = GuiButton((Rectangle) { 495, 300, 190, 45 }, "Add");
		if (added) {
			window_Active = false;
			Color colors[] = { RED, PURPLE, MAGENTA, DARKGREEN, DARKPURPLE ,BROWN, BEIGE };
			equation_arr[num_eq].color = colors[(GetRandomValue(0, 6))];
			equation_arr[num_eq].type = windows[current_category].types[current_type];
			equation_arr[num_eq].show = true;
			//equation_arr[num_eq].type.value = {0,0,0,0};
			for(int i=0; i< 4; i++)
				equation_arr[num_eq].type.value[i] = GetRandomValue(-10, 10);
			num_eq++;
		}
	}
}



int box_eq(int i)
{
	bool showEq = equation_arr[i].show;


	char label[80];
	strcpy(label,equation_arr[i].type.label);
	float a = equation_arr[i].type.value[0];
	float b = equation_arr[i].type.value[1];
	float c = equation_arr[i].type.value[2];
	float d = equation_arr[i].type.value[3];
	Color color = equation_arr[i].color;


	// Draw GUI controls
	//------------------------------------------------------------------------------
	equation_arr[i].show = GuiCheckBox((Rectangle) { 15, 15 + 120*i, 20, 20 }, "\0", equation_arr[i].show);
	DrawRectangleLines(50, 5 + 120 * i, 235, 40, BLACK);
	//DrawText(labelBuilder(label,a,b,c,d), 50 + 10,10 +120*i, 25, color);
	if(!strcmp(label, "cubic_x") || !strcmp(label, "cubic_y"))
		DrawTextEx(font_label, labelBuilder(label, a, b, c, d), (Vector2) { 45 + 10, 15 + 120 * i }, 19, 1, color);
	else
		DrawTextEx(font_label, labelBuilder(label, a, b, c, d), (Vector2) { 50 + 10, 12 + 120 * i }, 25, 1, color);
}

int draw_graph(int i)
{
	bool showEq = equation_arr[i].show;


	char label[80];
	strcpy(label, equation_arr[i].type.label);
	float a = equation_arr[i].type.value[0];
	float b = equation_arr[i].type.value[1];
	float c = equation_arr[i].type.value[2];
	float d = equation_arr[i].type.value[3];
	Color color = equation_arr[i].color;


	if (showEq)
	{
		show_eq(label, a, b, c, d,color);
		//DrawRectangle(0, 0, GetScreenWidth() - GetScreenHeight(), GetScreenHeight(), Fade(LIGHTGRAY, 0.6f));	//sliders(i);
	}
}

int clean_up() {
	UnloadFont(font_label);
	UnloadFont(font_main);
	UnloadSound(fx);
	CloseAudioDevice();
	CloseWindow();
};



char *labelBuilder(char *label, float a, float b, float c, float d){
	//polynomial
	if (!strcmp(label, "linear")) {
		if(!a&&!b) return (char*) TextFormat("y=0");
		if(!b) return (char*) TextFormat("y=%.1f",a);
		if(!a) return (char *)TextFormat("y=%.1f",b);
		if (b > 0) return (char *) TextFormat("y=%.1fx + %.1f", a, b);
		else return (char *)TextFormat("y=%.1fx - %.1f", a, -b);
	}


	if (!strcmp(label, "quadratic_x")) {
		if(!a&&!b&&!c) return (char *)TextFormat("y=0"); // a=b=c=0
		else if(!b&&!c) return (char *)TextFormat("y=%.1fx^2",a); //b=c=0
		else if(!a&&!c) return (char *)TextFormat("y=%.1fx",b); //a=c=0
		else if(!a&&!b) return (char *)TextFormat("y=%.1f",c); //a=b=0
		else if(!a) {
			if(c>0) return (char *)TextFormat("y=%.1fx + %.1f",b,c); // a=0 c>0
			else return (char *)TextFormat("y=%.1fx - %.1f",b,-1*c); //a=0 c<0
		}
		else if(!b) {
			if(c>0) return (char *)TextFormat("y=%.1fx^2 + %.1f",a,c); // b=0 c>0
			else return (char *)TextFormat("y=%.1fx^2 - %.1f",a,-1*c); // b=0 c<0
		}
		else if(!c) {
			if(b>0) return (char *)TextFormat("y=%.1fx^2 + %.1fx",a,b); // c=0 b>0
			else return (char *)TextFormat("y=%.1fx^2 - %.1fx",a,-1*b); // c=0 b<0
		}
		else{
			if(b>0 && c>0 ) return (char *)TextFormat("y=%.1fx^2 + %.1fx + %.1f", a, b, c); // b and c both positive
			else if(c>0) return (char *)TextFormat("y=%.1fx^2 - %.1fx + %.1f", a, -1*b, c); // b negative
			else if(b>0) return (char *)TextFormat("y=%.1fx^2 + %.1fx - %.1f", a, b, -1*c); // c negative
			else return (char *)TextFormat("y=%.1fx^2 - %.1fx - %.1f", a, -1*b, -1*c); // b and c both negative
			}
	}
	

	if (!strcmp(label, "quadratic_y")) {
		if(!a&&!b&&!c) return (char *)TextFormat("x=0"); // a=b=c=0
		else if(!b&&!c) return (char *)TextFormat("x=%.1fy^2",a); //b=c=0
		else if(!a&&!c) return (char *)TextFormat("x=%.1fy",b); //a=c=0
		else if(!a&&!b) return (char *)TextFormat("x=%.1f",c); //a=b=0
		else if(!a) {
			if(c>0) return (char *)TextFormat("x=%.1fy + %.1f",b,c); // a=0 c>0
			else return (char *)TextFormat("x=%.1fy - %.1f",b,-1*c); //a=0 c<0
		}
		else if(!b) {
			if(c>0) return (char *)TextFormat("x=%.1fy^2 + %.1f",a,c); // b=0 c>0
			else return (char *)TextFormat("x=%.1fy^2 - %.1f",a,-1*c); // b=0 c<0
		}
		else if(!c) {
			if(b>0) return (char *)TextFormat("x=%.1fy^2 + %.1fy",a,b); // c=0 b>0
			else return (char *)TextFormat("x=%.1fy^2 - %.1fy",a,-1*b); // c=0 b<0
		}
		else{
			if(b>0 && c>0 ) return (char *)TextFormat("x=%.1fy^2 + %.1fy + %.1f", a, b, c); // b and c both positive
			else if(c>0) return (char *)TextFormat("x=%.1fy^2 - %.1fy + %.1f", a, -1*b, c); // b negative
			else if(b>0) return (char *)TextFormat("x=%.1fy^2 + %.1fy - %.1f", a, b, -1*c); // c negative
			else return (char *)TextFormat("x=%.1fy^2 - %.1fy - %.1f", a, -1*b, -1*c); // b and c both negative
			}
	}

	if (!strcmp(label, "cubic_x")) {
		if(!a&&!b&&!c&&!d) return (char *)TextFormat("y=0"); 
		if(!b&&!c&&!d) return (char *)TextFormat("y=%.1fx^3", a);
		if(!a&&!c&&!d) return (char *)TextFormat("y=%.1fx^2", b); 
		if(!a&&!b&&!d) return (char *)TextFormat("y=%.1fx", c); 
		if(!a&&!b&&!c) return (char *)TextFormat("y=%.1f", d); 
		if(!a&&!b) {
			if(d>0) return (char *)TextFormat("y=%.1fx + %.1f", c, d);
			else return (char *)TextFormat("y=%.1fx + %.1f", c, d);
		}
		if(!a&&!c) {
			if(d>0) return (char *)TextFormat("y=%.1fx^2 + %.1f",  b,  d);
			else return (char *)TextFormat("y=%.1fx^2 - %.1f", b, -1*d);
		}
		if(!a&&!d) {
			if(c>0) return (char *)TextFormat("y=%.1fx^2 + %.1fx",  b,  c);
			else return (char *)TextFormat("y=%.1fx^2 - %.1fx", b, -1*c);
		}
		if(!b&&!d) {
			if(c>0) return (char *)TextFormat("y=%.1fx^3 + %.1fx",  a,  c);
			else return (char *)TextFormat("y=%.1fx^3 - %.1fx", a, -1*c);
		}
		if(!b&&!c) {
			if(d>0) return (char *)TextFormat("y=%.1fx^3 + %.1f",  a,  d);
			else return (char *)TextFormat("y=%.1fx^3 - %.1f", a, -1*d);
		}
		if(!c&&!d) {
			if(b>0) return (char *)TextFormat("y=%.1fx^3 + %.1fx^2",  a,  b);
			else return (char *)TextFormat("y=%.1fx^3 - %.1fX^2", a, -1*b);
		}
		if(!a) {
			if(c<0&&d<0) return (char *)TextFormat("y=%.1fx^2 - %.1fx - %.1f", b, -1*c, -1*d);
			if(c<0) return (char *)TextFormat("y=%.1fx^2 - %.1fx + %.1f", b, -1*c, d);
			if(d<0) return (char *)TextFormat("y=%.1fx^2 + %.1fx - %.1f", b, c, -1*d);
			else return (char *)TextFormat("y=%.1fx^2 + %.1fx + %.1f", b, c, d);
		};
		if(!b) {
			if(c<0&&d<0) return (char *)TextFormat("y=%.1fx^3 - %.1fx - %.1f", a, -1*c, -1*d);
			if(c<0) return (char *)TextFormat("y=%.1fx^3 - %.1fx + %.1f", a, -1*c, d);
			if(d<0) return (char *)TextFormat("y=%.1fx^3 + %.1fx - %.1f", a, c, -1*d);
			else return (char *)TextFormat("y=%.1fx^3 + %.1fx + %.1f", a, c, d);
		}
		if(!c){
			if(b<0&&d<0) return (char *)TextFormat("y=%.1fx^3 - %.1fx^2 - %.1f", a, -1*b, -1*d);
			if(b<0) return (char *)TextFormat("y=%.1fx^3 - %.1fx^2 + %.1f", a, -1*b, d);
			if(d<0) return (char *)TextFormat("y=%.1fx^3 + %.1fx^2 - %.1f", a, b, -1*d);
			else return (char *)TextFormat("y=%.1fx^3 + %.1fx^2 + %.1f", a, b, d);
		}
		if(!d) {
			if(b<0&&c<0) return (char *)TextFormat("y=%.1fx^3 - %.1fx^2 - %.1fx", a, -1*b, -1*c);
			if(b<0) return (char *)TextFormat("y=%.1fx^3 - %.1fx^2 + %.1fx", a, -1*b, c);
			if(c<0) return (char *)TextFormat("y=%.1fx^3 + %.1fx^2 - %.1fx", a, b, -1*c);
			else return (char *)TextFormat("y=%.1fx^3 + %.1fx^2 + %.1fx", a, b, c);
		}; 

			if(b<0&&c<0&&d<0) return (char *)TextFormat("y=%.1fx^3 - %.1fx^2 - %.1fx - %.1f", a, -1*b, -1*c,-1*d);
			if(b<0&&c<0) return (char *)TextFormat("y=%.1fx^3 - %.1fx^2 - %.1fx + %.1f", a, -1*b, -1*c,d);
			if(b<0&&d<0) return (char *)TextFormat("y=%.1fx^3 - %.1fx^2 + %.1fx - %.1f", a, -1*b, c,-1*d);
			if(c<0&&d<0) return (char *)TextFormat("y=%.1fx^3 + %.1fx^2 - %.1fx - %.1f", a, b, -1*c,-1*d);
			if(b<0) return (char *)TextFormat("y=%.1fx^3 - %.1fx^2 + %.1fx + %.1f", a, -1*b, c, d);
			if(c<0) return (char *)TextFormat("y=%.1fx^3 + %.1fx^2 - %.1fx + %.1f", a, b, -1*c,d);
			if(d<0) return (char *)TextFormat("y=%.1fx^3 + %.1fx^2 + %.1fx - %.1f", a, b, c,-1*d);
			else return (char *)TextFormat("y=%.1fx^3 + %.1fx^2 + %.1fx + %.1f", a, b, c,d);
		
	}

	if (!strcmp(label, "cubic_y")) {
		if(!a&&!b&&!c&&!d) return (char *)TextFormat("x=0");
		if(!b&&!c&&!d) return (char *)TextFormat("x=%.1fy^3", a);
		if(!a&&!c&&!d) return (char *)TextFormat("x=%.1fy^2", b);
		if(!a&&!b&&!d) return (char *)TextFormat("x=%.1fy", c);
		if(!a&&!b&&!c) return (char *)TextFormat("x=%.1f", d);
		if(!a&&!b) {
			if(d>0) return (char *)TextFormat("x=%.1fy + %.1f", c, d);
			else return (char *)TextFormat("x=%.1fy + %.1f", c, d);
		}
		if(!a&&!c) {
			if(d>0) return (char *)TextFormat("x=%.1fy^2 + %.1f",  b,  d);
			else return (char *)TextFormat("x=%.1fy^2 - %.1f", b, -1*d);
		}
		if(!a&&!d) {
			if(c>0) return (char *)TextFormat("x=%.1fy^2 + %.1fy",  b,  c);
			else return (char *)TextFormat("x=%.1fy^2 - %.1fy", b, -1*c);
		}
		if(!b&&!d) {
			if(c>0) return (char *)TextFormat("x=%.1fy^3 + %.1fy",  a,  c);
			else return (char *)TextFormat("x=%.1fy^3 - %.1fy", a, -1*c);
		}
		if(!b&&!c) {
			if(d>0) return (char *)TextFormat("x=%.1fy^3 + %.1f",  a,  d);
			else return (char *)TextFormat("x=%.1fy^3 - %.1f", a, -1*d);
		}
		if(!c&&!d) {
			if(b>0) return (char *)TextFormat("x=%.1fy^3 + %.1fy^2",  a,  b);
			else return (char *)TextFormat("x=%.1fy^3 - %.1fy^2", a, -1*b);
		}
		if(!a) {
			if(c<0&&d<0) return (char *)TextFormat("x=%.1fy^2 - %.1fy - %.1f", b, -1*c, -1*d);
			if(c<0) return (char *)TextFormat("x=%.1fy^2 - %.1fy + %.1f", b, -1*c, d);
			if(d<0) return (char *)TextFormat("x=%.1fy^2 + %.1fy - %.1f", b, c, -1*d);
			else return (char *)TextFormat("x=%.1fy^2 + %.1fy + %.1f", b, c, d);
		};
		if(!b) {
			if(c<0&&d<0) return (char *)TextFormat("x=%.1fy^3 - %.1fy - %.1f", a, -1*c, -1*d);
			if(c<0) return (char *)TextFormat("x=%.1fy^3 - %.1fy + %.1f", a, -1*c, d);
			if(d<0) return (char *)TextFormat("x=%.1fy^3 + %.1fy - %.1f", a, c, -1*d);
			else return (char *)TextFormat("x=%.1fy^3 + %.1fy + %.1f", a, c, d);
		}
		if(!c){
			if(b<0&&d<0) return (char *)TextFormat("x=%.1fy^3 - %.1fy^2 - %.1f", a, -1*b, -1*d);
			if(b<0) return (char *)TextFormat("x=%.1fy^3 - %.1fy^2 + %.1f", a, -1*b, d);
			if(d<0) return (char *)TextFormat("x=%.1fy^3 + %.1fy^2 - %.1f", a, b, -1*d);
			else return (char *)TextFormat("x=%.1fy^3 + %.1fy^2 + %.1f", a, b, d);
		}
		if(!d) {
			if(b<0&&c<0) return (char *)TextFormat("x=%.1fy^3 - %.1fy^2 - %.1fy", a, -1*b, -1*c);
			if(b<0) return (char *)TextFormat("x=%.1fy^3 - %.1fy^2 + %.1fy", a, -1*b, c);
			if(c<0) return (char *)TextFormat("x=%.1fy^3 + %.1fy^2 - %.1fy", a, b, -1*c);
			else return (char *)TextFormat("x=%.1fy^3 + %.1fy^2 + %.1fy", a, b, c);
		}; 
	
			if(b<0&&c<0&&d<0) return (char *)TextFormat("x=%.1fy^3 - %.1fy^2 - %.1fy - %.1f", a, -1*b, -1*c,-1*d);
			if(b<0&&c<0) return (char *)TextFormat("x=%.1fy^3 - %.1fy^2 - %.1fy + %.1f", a, -1*b, -1*c,d);
			if(b<0&&d<0) return (char *)TextFormat("x=%.1fy^3 - %.1fy^2 + %.1fy - %.1f", a, -1*b, c,-1*d);
			if(c<0&&d<0) return (char *)TextFormat("x=%.1fy^3 + %.1fy^2 - %.1fy - %.1f", a, b, -1*c,-1*d);
			if(b<0) return (char *)TextFormat("x=%.1fy^3 - %.1fy^2 + %.1fy + %.1f", a, -1*b, c, d);
			if(c<0) return (char *)TextFormat("x=%.1fy^3 + %.1fy^2 - %.1fy + %.1f", a, b, -1*c,d);
			if(d<0) return (char *)TextFormat("x=%.1fy^3 + %.1fy^2 + %.1fy - %.1f", a, b, c,-1*d);
			else return (char *)TextFormat("x=%.1fy^3 + %.1fy^2 + %.1fy + %.1f", a, b, c,d);
	}

	// // Conic Section
	if (!strcmp(label, "hyperbola")){
		return (char *) TextFormat("x/%.1f - y/%.1f=1",a*a,b*b);
	}

	if (!strcmp(label, "ellipse")){
		return (char *) TextFormat("x/%.1f + y/%.1f=1",a*a,b*b);
	}

	if (!strcmp(label, "circle")){
		return (char *) TextFormat("x^2 + y^2=(%.1f)^2",a,b);
	}

	if(!strcmp(label, "parabola")){
		return (char *) TextFormat("y^2=%.1fx", 4*a);
	}

	// trignometric
	if (!strcmp(label, "sin")){
		if(!a) return (char *)TextFormat("y=0");
		else if(!b) return (char *)TextFormat("y=%.1fsin(%.1f)",a,c);
		else if(!c) return (char *)TextFormat("y=%.1fsin(%.1fx)",a,b);
		else if(c<0) return (char *)TextFormat("y=%.1fsin(%.1fx - %.1f)",a,b,-1*c);
		else return (char *)TextFormat("y=%.1fsin(%.1fx + %.1f)",a,b,c);
	}

	if (!strcmp(label, "cos")){
		if(!a) return (char *)TextFormat("y=0");
		else if(!b) return (char *)TextFormat("y=%.1fcos(%.1f)",a,c);
		else if(!c) return (char *)TextFormat("y=%.1fcos(%.1fx)",a,b);
		else if(c<0) return (char *)TextFormat("y=%.1fcos(%.1fx - %.1f)",a,b,-1*c);
		else return (char *)TextFormat("y=%.1fcos(%.1fx + %.1f)",a,b,c);
	}
	 
	if (!strcmp(label, "tan")){
		if(!a) return (char *)TextFormat("y=0");
		else if(!b) return (char *)TextFormat("y=%.1ftan(%.1f)",a,c);
		else if(!c) return (char *)TextFormat("y=%.1ftan(%.1fx)",a,b);
		else if(c<0) return (char *)TextFormat("y=%.1ftan(%.1fx - %.1f)",a,b,-1*c);
		else return (char *)TextFormat("y=%.1ftan(%.1fx + %.1f)",a,b,c);
	}

	// // exponential
	if (!strcmp(label, "log_a_x")){
		if(a<=0|| a ==1) return (char *)TextFormat("doesn't exist!");
		else return (char *)TextFormat("y=log%.1f(x)",a);
	}

	if (!strcmp(label, "a_x")){
		return (char *)TextFormat("y=(%.1f)^x",a);
	}

	if (!strcmp(label, "e_ax")){
		return (char *)TextFormat("y=(e)^%.1fx",a);
	}
	
	return label;
}



