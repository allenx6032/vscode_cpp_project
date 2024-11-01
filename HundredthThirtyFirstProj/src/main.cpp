#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <memory>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <locale.h>
#include <random>
#include <vector>
#include <chrono>
#include <Windows.h>
using namespace std;

// #include <blah.h>
// using namespace Blah;

#include "httplib.h"
//using namespace httplib;

#include "md5.h"
#include "json.hpp"
#include "tigr.h"
#include "SDL.h"
#include "wz.h"

#ifdef _WIN32
#include <winsock2.h>
#ifdef _MSC_VER
#include <gl2.h>
#else
#include <GL/gl.h>
#endif
#elif defined __linux__
#include <GL/gl.h>
#else
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#endif

// Batch batch;
#define WINDOW_WIDTH 200
#define WINDOW_HEIGHT 150

class TigrRenderer : public wz::IRenderer
{
public:
	TigrRenderer(Tigr *screen) : screen(screen) {}
	virtual void drawButton(wz::Button *button, wz::Rect /*clip*/)
	{
		const wz::Rect rect = button->getAbsoluteRect();
		// Change color when hovering.
		const TPixel color = button->getHover() ? tigrRGB(128, 255, 128) : tigrRGB(255, 255, 255);
		// Draw background and border.
		tigrFill(screen, rect.x, rect.y, rect.w, rect.h, tigrRGB(64, 64, 64));
		tigrRect(screen, rect.x, rect.y, rect.w, rect.h, color);
		// Draw centered text.
		wz::Size textSize;
		textSize.w = tigrTextWidth(tfont, button->getLabel());
		textSize.h = tigrTextHeight(tfont, button->getLabel());
		tigrPrint(screen, tfont, rect.x + rect.w / 2 - textSize.w / 2, rect.y + rect.h / 2 - textSize.h / 2, color, button->getLabel());
	}
	virtual wz::Size measureButton(wz::Button *button)
	{
		// Measure text.
		wz::Size size;
		size.w = tigrTextWidth(tfont, button->getLabel());
		size.h = tigrTextHeight(tfont, button->getLabel());
		// Add padding.
		const wz::Border padding = button->getPadding();
		size.w += padding.left + padding.right;
		size.h += padding.top + padding.bottom;
		return size;
	}
private:
	Tigr *screen;
};

struct StopWatch {
  StopWatch(const string &label) : label_(label) {
    start_ = chrono::system_clock::now();
  }
  ~StopWatch() {
    auto end = chrono::system_clock::now();
    auto diff = end - start_;
    auto count = chrono::duration_cast<chrono::milliseconds>(diff).count();
    cout << label_ << ": " << count << " millisec." << endl;
  }
  string label_;
  chrono::system_clock::time_point start_;
};

#undef main
int main(int argc, char* argv[])
{
    // httplib::Server svr;
    // svr.Get("/", [](const httplib::Request &, httplib::Response &res) {
    //     res.set_content("Hello World!", "text/plain");
    //     // break;
    // });
    // svr.listen("0.0.0.0", 8080);
    //无参调用POST接口
    //httplib::Result Client::Post(const char *path);
    //带参数调用POST接口， 参数一般是Json形式的
    //httplib::Result Client::Post(const char *path, const Params &params);	
    //带Header、参数调用POST接口
    //httplib::Result Client::Post(const char *path, const Headers &headers,const Params &params);
    //不带Header、表单参数调用POST接口
    //httplib::Result Client::Post(const char *path, const MultipartFormDataItems &items);
    //带Header、表单参数调用Post参数
    //httplib::Result Client::Post(const char *path, const Headers &headers, const MultipartFormDataItems &items); 
    string body(1024 * 5, 'a');
    httplib::Client clip("httpbin.org", 80);
    httplib::Params params;
    params.emplace("name", "john");
    params.emplace("note", "coder");
    for (int i = 0; i < 3; i++) {
        StopWatch sw(to_string(i).c_str());
        // auto res = clip.Post("/post", params, "application/octet-stream");
        auto res = clip.Post("/post", body, "application/octet-stream");
        assert(res->status == httplib::StatusCode::OK_200);
    }

    Tigr* win1 = tigrWindow(200, 150, "Window1", 0);
    Tigr* win2 = tigrWindow(200, 150, "Window2", 0);
    while (win1 || win2) {
        if (win1) {
            tigrClear(win1, tigrRGB(0x80, 0x90, 0xa0));
            tigrPrint(win1, tfont, 50, 50, tigrRGB(0xff, 0xff, 0xff), "Hello, world #1.");
            tigrUpdate(win1);
            if (tigrClosed(win1) || tigrKeyDown(win1, TK_ESCAPE)) {
                tigrFree(win1);
                win1 = NULL;
            }
        }
        if (win2) {
            if (tigrBeginOpenGL(win2)) {
                glClearColor(1, 0, 1, 1);
                glClear(GL_COLOR_BUFFER_BIT);
            }
            tigrClear(win2, tigrRGBA(0x00, 0x00, 0x00, 0x00));
            tigrPrint(win2, tfont, 50, 50, tigrRGB(0xff, 0xff, 0xff), "Hello, world #2.");
            tigrUpdate(win2);
            if (tigrClosed(win2) || tigrKeyDown(win2, TK_ESCAPE)) {
                tigrFree(win2);
                win2 = NULL;
            }
        }
    }
    if (win1)
        tigrFree(win1);
    if (win2)
        tigrFree(win2);

    // Config config;
    // config.name = "blah app";
    // config.on_render = []()
    // {
    //     auto target = App::backbuffer();
    //     target->clear(Color::black);

    //     auto center = Vec2f(target->width(), target->height()) / 2;
    //     auto rotation = Time::seconds * Calc::TAU;
    //     auto transform = Mat3x2f::create_transform(center, Vec2f::zero, Vec2f::one, rotation);

    //     batch.push_matrix(transform);
    //     batch.rect(Rectf(-32, -32, 64, 64), Color::red);
    //     batch.pop_matrix();

    //     batch.render(target);
    //     batch.clear();
    // };
    // App::run(&config);


    //Create the tigr window.
	Tigr *tigrScreen = tigrWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "WidgetZero Example - Custom Renderer", 0);
	// Create the custom renderer.
	TigrRenderer *tigrRenderer = new TigrRenderer(tigrScreen);
	// Create the main window.
	wz::MainWindow *mainWindow = new wz::MainWindow(tigrRenderer);
	mainWindow->setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	// Create a button.
	wz::Button *button = new wz::Button("Click me!");
	button->setAlign(wz::Align::Center | wz::Align::Middle);
	mainWindow->add(button);
	// For tracking input state changes.
	int lastMouseX = 0, lastMouseY = 0, lastMouseButtons = 0;
	while (!tigrClosed(tigrScreen))
	{
		// Handle mouse movement.
		int mouseX, mouseY, mouseButtons;
		tigrMouse(tigrScreen, &mouseX, &mouseY, &mouseButtons);
		if (mouseX != lastMouseX || mouseY != lastMouseY)
		{
			mainWindow->mouseMove(mouseX, mouseY, mouseX - lastMouseX, mouseY - lastMouseY);
		}
		lastMouseX = mouseX;
		lastMouseY = mouseY;
		lastMouseButtons = mouseButtons;
		// Draw.
		tigrClear(tigrScreen, tigrRGB(192, 192, 192));
		mainWindow->draw();
		tigrUpdate(tigrScreen);
	}
	tigrFree(tigrScreen);

    return 0;
}