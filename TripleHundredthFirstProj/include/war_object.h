#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED

#include <SFML_Graphics.hpp>

#include "war_content.h"
#include "war_vao.h"

class Object
{
    public:
        sf::Vector2i position;
        unsigned char rotation;
        std::string contentID;
        //VAO mesh;

        Object();
        Object(int x, int y, int rot, std::string ID);

        void update(const float & dt, Content * content);
        void render(sf::RenderTarget * target, Content * content);
};

#endif // OBJECT_H_INCLUDED

