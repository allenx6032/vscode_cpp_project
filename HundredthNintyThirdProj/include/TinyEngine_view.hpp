#ifndef TINYENGINE_VIEW
#define TINYENGINE_VIEW

#include <imgui.h>                    //Interface Dependencies
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>

class View {
  public:
    bool init(std::string windowName, int width, int height);
    void quit();
    bool enabled = false;

    unsigned int WIDTH, HEIGHT;

    SDL_Window* gWindow;        //Window Pointer
    SDL_GLContext gContext;     //Render Context

    ImGuiIO io;
    Handle interface = [](){};  //User defined Interface
    bool showInterface = false;
    void drawInterface();

    Handle pipeline = [](){};           //User defined Pipeline
    void render();
    void target(glm::vec3 clearcolor, bool clearc = true, bool cleard = true);  //Target main window for drawing

    bool windowed = false;
    bool fullscreen = false;    //Settings
    bool vsync = true;
    bool ccw = true;
    bool blend = true;
    unsigned int antialias = 16;
    float lineWidth = 1.0f;
    float pointSize = 1.0f;
};

#endif
