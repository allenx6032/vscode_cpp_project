#pragma once

#include "deceptus_layer.h"

#include <SFML_Graphics.hpp>
#include <SFML_System.hpp>

#include <map>
#include <memory>

class ForestScene
{
public:
   ForestScene();

   void draw(sf::RenderTarget& window, sf::RenderStates = sf::RenderStates::Default);
   void update(const sf::Time& time);

private:
   std::vector<std::shared_ptr<Layer>> _layer_stack;
   std::map<std::string, std::shared_ptr<Layer>> _layers;

   sf::Font _font;
   std::unique_ptr<sf::Text> _text;
};
