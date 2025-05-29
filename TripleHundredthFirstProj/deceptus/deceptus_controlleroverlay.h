#pragma once

#include "deceptus_layer.h"

#include <SFML_Graphics.hpp>
#include <SFML_System.hpp>

#include <map>
#include <memory>
#include <string>

class ControllerOverlay
{
public:
   ControllerOverlay();
   void draw(sf::RenderTarget& window, sf::RenderStates = sf::RenderStates::Default);

private:
   std::map<std::string, std::shared_ptr<Layer>> _layers;
   sf::Vector2i _texture_size;
};
