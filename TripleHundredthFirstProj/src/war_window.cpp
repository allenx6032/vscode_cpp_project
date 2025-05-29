#include "war_window.h"

#include <string>

sf::Window window;
sf::Vector2i window_center;

void InitWindow()
{
    std::string title = "War";
    bool vertical_sync_enabled = false;

    sf::ContextSettings context_settings;
    context_settings.depthBits         = 24; // Request a 24 bits depth buffer
    context_settings.stencilBits       = 8;  // Request a 8 bits stencil buffer
    context_settings.antialiasingLevel = 2;  // Request 2 levels of antialiasing

    window.create(sf::VideoMode(1120, 630), title, sf::Style::Default, context_settings);
    window.setVerticalSyncEnabled(vertical_sync_enabled);

    window_center = sf::Vector2i(window.getSize() / 2u);
}
