#include <iostream>
#include <bongo_lua.hpp>  // bongo lua
#include <bongo_sfml.hpp> // sfml lua bindings

#include <bongo_audio.hpp>
#include <bongo_global.hpp>
#include <bongo_helper.hpp>
#include <bongo_input.hpp>
#include <bongo_sprite.hpp>
#include <bongo_window.hpp>

sf::RenderWindow main_window;
sf::RenderStates main_render_states; // may cause artifacts when switching cats
lua_State *LuaState;

#if defined(_WIN32)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    FreeConsole(); // maybe not the best option as the console is still running
                   // with the console subsystem, but it does hide the console
#else
int main() {
#endif
    BongoLua::initializeLua();

    BongoInput::init();

    BongoSfml::bindToLua();
    BongoWindow::bindToLua();
    BongoSprite::bindToLua();
    BongoInput::bindToLua();
    BongoAudio::bindToLua();
    BongoHelper::bindToLua();

    BongoLua::executeScript("src/test.lua");
    BongoLua::closeLua();

    return 0;
}
