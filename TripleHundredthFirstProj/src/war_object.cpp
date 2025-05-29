#include "war_object.h"

Object::Object()
{

}

Object::Object(int x, int y, int rot, std::string ID)
{
    position = sf::Vector2i(x, y);
    rotation = rot;
    contentID = ID;
}

void Object::update(const float & dt, Content * content)
{

}

void Object::render(sf::RenderTarget * target, Content * content)
{
    //mesh.Draw();
}

