#ifndef VECTOR_FUNC_HPP
#define VECTOR_FUNC_HPP
#include <cmath>
#include <SFML_Graphics.hpp>
const float NORMAL_TOLERANCE = 0.0001f;
inline float sqr(float val)
{
	return val * val;
}
inline float getLength(const sf::Vector2f& vector)
{
	return sqrt(sqr(vector.x) + sqr(vector.y));
}
inline float dotProduct(const sf::Vector2f& v1, const sf::Vector2f& v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}
inline sf::Vector2f getNormal(const sf::Vector2f& vector)
{
	return sf::Vector2f(-vector.y, vector.x);
}
inline sf::Vector2f getNormalized(const sf::Vector2f& vector)
{
	float length = getLength(vector);
	if (length < NORMAL_TOLERANCE)
		return sf::Vector2f();
	return sf::Vector2f(vector.x / length, vector.y / length);
}
inline sf::Vector2f getGlobalCenter(const sf::Shape& shape)
{
	const sf::Transform& transform = shape.getTransform();
	sf::FloatRect local = shape.getLocalBounds();
	return transform.transformPoint(local.left + local.width / 2.f, local.top + local.height / 2.f);
}
#endif //VECTOR_FUNC_HPP