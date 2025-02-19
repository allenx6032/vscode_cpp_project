#include <application.h>
#include<SDL_GUI_core.h>
#include<SDL_GUI_example_plugin.h>

using namespace SDL_GUI;

int main(int argc, char *argv[]) {
    Application<Core, ExamplePlugin> app{"Application", argc, argv};
    app.run();
    return 0;
}
