#ifndef OBJECT_CONTENT_H_INCLUDED
#define OBJECT_CONTENT_H_INCLUDED

#include <SFML_Graphics.hpp>

struct ObjectContent
{
    sf::Texture * texture_up;
    sf::Texture * texture_down;
    sf::Texture * texture_left;
    sf::Texture * texture_right;
    sf::Vector2f spriteSize;
    sf::Vector2f spriteOrigin;
    //sf::Vector2u collision;
    //Vector2u collisionOrigin;
};

#endif // OBJECT_CONTENT_H_INCLUDED

