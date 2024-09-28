#ifndef COLLISION_HPP_
#define COLLISION_HPP_
#include <SFML_Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>
#include <sstream>
#include "2dge_luaScript.hpp"
#define NDEBUG
#ifndef NDEBUG
#define __DEBUG_EXEC(code) code
#else
#define __DEBUG_EXEC(code) ;
#endif
enum COLLIDE_MODE 
{
	CIRCLE_MODE = 0,
	RECT_MODE, 
	CONVEX_MODE,
};
class Collider
{
	sf::CircleShape circle;
	sf::RectangleShape rectangle;
	sf::ConvexShape convex;
	sf::Text info;
	sf::Font font;
public:
	Collider();
	~Collider();
	const sf::CircleShape& getCircle() const;
	const sf::RectangleShape& getRect() const;
	const sf::ConvexShape& getConvex() const;
	void setRectPosition(float x, float y);
	void setRectPosition(const sf::Vector2f& position);
	void setConvexPosition(float x, float y);
	void setConvexPosition(const sf::Vector2f& vec);
	void setCirclePosition(float x, float y);
	void setCirclePosition(const sf::Vector2f& vec);
	void setCircleCenter(float x, float y);
	void setCircleCenter(const sf::Vector2f& vec);
	void setPosition(float x, float y);
	void setPosition(const sf::Vector2f& vec);
	void setRect(const sf::FloatRect& fRect);
	void setRect(const sf::IntRect& iRect);
	void setRectSize(const sf::Vector2f &size);
	void setOrigin(float x, float y);
	void setOrigin(sf::Vector2f vec);
	void setRotation(float angle);
	void setCircleRadius(float radius);
	void move(float x, float y);
	void move(const sf::Vector2f& vec);
	void infoUpdate(float x, float y);
	void infoUpdate(const sf::Vector2f& vec);
	void setConvexPointCount(std::size_t count);
	void setConvexPoint(std::size_t index, const sf::Vector2f& point);
	const sf::Text& getInfo() const;
};
class Collision
{
public:
	Collision();
	~Collision();
	bool intersect(const sf::CircleShape& circle1, const sf::CircleShape& circle2, sf::Vector2f* mtv_ptr = nullptr);
	bool intersect(const sf::RectangleShape& rectangle1, const sf::RectangleShape& rectangle2, sf::Vector2f* mtv_ptr = nullptr);
	bool intersect(const sf::ConvexShape& convex1, const sf::ConvexShape& convex2, sf::Vector2f* mtv_ptr = nullptr);
	bool isCollide(const Collider& collider1, const Collider& collider2, int collideMode, sf::Vector2f* mtv_ptr = nullptr);
};
#endif //COLLISION_HPP_