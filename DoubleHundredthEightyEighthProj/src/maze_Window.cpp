#include "maze_Window.h"
#include "raylib.h"
using std::string;

// Constructor definition
Window::Window(int window_width, int window_height, string window_title)
    : __win_title(window_title), __win_width(window_width),
      __win_height(window_height) {
  this->init();
};

Window::~Window() { CloseWindow(); }

Window::Window(string window_title)
    : __win_title(window_title), __win_width(0), __win_height(0) {
  this->init();
};

// default init function
void Window::init() {
  InitWindow(this->__win_width, this->__win_height, this->__win_title.c_str());
  SetWindowState(FLAG_VSYNC_HINT);
}

// Check if application should close (KEY_ESCAPE pressed or windows close icon
// clicked)
bool Window::ShouldClose() { return ::WindowShouldClose(); }

// Setup init configuration flags (view FLAGS)
void Window::SetConfigFlags(unsigned int flags) { SetConfigFlags(flags); }

// Set a custom key to exit program (default is ESC)
void Window::SetExitKey(int key) { SetExitKey(key); }

// Return the Window height
int Window::windowHeight() { return GetScreenHeight(); }

// Return the Window Width
int Window::windowWidth() { return GetScreenHeight(); }

int Window::getWindowWidth() {
  if (__win_width == 0) {
    return GetScreenWidth();
  } else {
    return this->__win_width;
  }
}

int Window::getWindowHeight() {
  if (__win_height == 0) {
    return GetScreenHeight();
  } else {
    return this->__win_height;
  }
}
