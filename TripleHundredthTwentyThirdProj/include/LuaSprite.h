#pragma once

#include <array>
#include <SFML_Vertex.hpp>
#include <SFML_Texture.hpp>
#include <SFML_RenderWindow.hpp>
#include "LuaIncludes.h"

class LuaSprite {
  public:
    void init(float width, float height, float x, float y);
    void draw(sf::RenderWindow& window);

    void move(float x, float y);
    const sf::Vector2f& getPosition() const;
    void setPosition(float x, float y);
    bool intersecting(LuaSprite& other) const;
    void setTexture(const sf::Texture& texture);

    static void luaRegister(lua_State* L);

  private:
    sf::FloatRect bounds() const; 
    std::array<sf::Vertex, 4> m_verts;

    sf::Vector2f m_position;
    sf::Vector2f m_size;

    const sf::Texture* m_texture;
};