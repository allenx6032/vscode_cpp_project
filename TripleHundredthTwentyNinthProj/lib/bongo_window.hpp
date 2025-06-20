#ifndef BONGO_CAT_WINDOW_HPP
#define BONGO_CAT_WINDOW_HPP
#include <bongo_global.hpp>

// should really be moved into the namespace
//============================================================================
// PREPROCESSORS
//============================================================================
extern sf::Vector2i grabbed_offset;
extern bool grabbed_window;

namespace BongoWindow {
//============================================================================
// CREATE
//============================================================================
void create();
void create(UInt32Ref style);
void create(UIntRef width, UIntRef height);
void create(UIntRef width, UIntRef height, UInt32Ref style);
void create(UIntRef width, UIntRef height, UInt32Ref style, Vec2iRef position);

//============================================================================
// GET WINDOW SIZE
//============================================================================
unsigned int getWidth();
unsigned int getHeight();
std::array<unsigned int, 2> getSize();

//============================================================================
// GET WINDOW POSITION
//============================================================================
unsigned int getX();
unsigned int getY();
std::array<int, 2> getPosition();

//============================================================================
// SET WINDOW POSITION
//============================================================================
void setX(int);
void setY(int);
void setPosition(sf::Vector2i);

//============================================================================
// REFRESH
//============================================================================
void refresh();
void refresh(UIntRef width, UIntRef height, UInt32Ref style);
void refresh(UIntRef width, UIntRef height, UInt32Ref style, Vec2iRef position);

//============================================================================
// PROCESS EVENTS
//============================================================================
int processEvents();

//============================================================================
// CLEAR
//============================================================================
void clear();
void clear(const sf::Color &color);

//============================================================================
// DRAW
//============================================================================
void draw(const sf::Sprite &sprite);
void draw(const sf::Drawable &drawable);
void draw(const sf::Vertex *vertices, size_t vertex_count,
          sf::PrimitiveType type);

//============================================================================
// DRAW IF
//============================================================================
void drawif(const sf::Sprite &sprite, bool condition);
void drawif(const sf::Drawable &drawable, bool condition);

//============================================================================
// DRAW IF ELSE
//============================================================================
void drawifelse(const sf::Sprite &true_sprite, bool condition,
                const sf::Sprite &false_sprite);
void drawifelse(const sf::Drawable &true_drawable, bool condition,
                const sf::Drawable &false_drawable);

//============================================================================
// DISPLAY
//============================================================================
void display();

//============================================================================
// CLOSE
//============================================================================
void close();

//============================================================================
// BIND TO LUA
//============================================================================
void bindToLua();
} // namespace BongoWindow

#endif
