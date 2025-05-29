#pragma once

#include <SFML_Graphics.hpp>

struct Hitbox
{
   Hitbox(const sf::FloatRect& rect, const sf::Vector2f& offset);
   sf::FloatRect getRectTranslated() const;

   sf::FloatRect _rect_px;
   sf::Vector2f _offset_px;
};
