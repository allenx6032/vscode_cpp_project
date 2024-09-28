#include "2dge_collision.hpp"
#include "2dge_vector_func.hpp"
static const std::size_t CHARACTER_SIZE = 10;
static const std::size_t OUTLINE_THICKNESS = 1;
static inline sf::Vector2f getPerpendicularAxis(const sf::Vector2f& vertex1, const sf::Vector2f& vertex2)
{
	return getNormal(getNormalized(vertex2 - vertex1));
}
static inline bool areOverlapping(const sf::Vector2f& v1, const sf::Vector2f& v2)
{
	return v1.x <= v2.y && v1.y >= v2.x;
}
static inline float getOverlapLength(const sf::Vector2f& v1, const sf::Vector2f& v2)
{
	if (!areOverlapping(v1, v2))
		return 0.f;
	return std::min(v1.y, v2.y) - std::max(v1.x, v2.x);
}
static sf::Vector2f projectOnAxis(const std::vector<sf::Vector2f>& vertices, const sf::Vector2f& axis)
{
	float min = std::numeric_limits<float>::infinity();
	float max = -std::numeric_limits<float>::infinity();
	for (auto& vertex: vertices) {
		float projection = dotProduct(vertex, axis);
		if (projection < min)
			min = projection;
		if (projection > max)
			max = projection;
	}
	return sf::Vector2f(min, max);
}
static std::vector<sf::Vector2f> getVertices(const sf::ConvexShape& convex)
{
	std::vector<sf::Vector2f> vertices;
	const sf::Transform& transform = convex.getTransform();
	auto pointCount = convex.getPointCount();
	assert(pointCount >= 3);
	for (std::size_t i = 0; i < pointCount; i++) {
		vertices.push_back(transform.transformPoint(convex.getPoint(i)));
	}
	return vertices;
}
static std::vector<sf::Vector2f> getPerpendicularAxes(const std::vector<sf::Vector2f>& vertex1, const std::vector<sf::Vector2f>& vertex2)
{
	std::vector<sf::Vector2f> axes;
	auto pointCount1 = vertex1.size();
	assert(pointCount1 >= 3);
	auto pointCount2 = vertex2.size();
	assert(pointCount2 >= 3);
	for (std::size_t i = 1; i < pointCount1; i++) {
		axes.push_back(getPerpendicularAxis(vertex1[i], vertex1[i - 1]));
	}
	axes.push_back(getPerpendicularAxis(vertex1[0],vertex1[pointCount1 - 1]));
	for (std::size_t i = 1; i < pointCount2; i++) {
		axes.push_back(getPerpendicularAxis(vertex2[i], vertex2[i - 1]));
	}
	axes.push_back(getPerpendicularAxis(vertex2[0], vertex2[pointCount2 - 1]));
	return axes;
}
Collider::Collider()
{
	__DEBUG_EXEC(std::cout << "Collider()\n");
	rectangle.setFillColor(sf::Color::Transparent);
	convex.setFillColor(sf::Color::Transparent);
	circle.setFillColor(sf::Color::Transparent);
	rectangle.setOutlineThickness(OUTLINE_THICKNESS);
	convex.setOutlineThickness(OUTLINE_THICKNESS);
	circle.setOutlineThickness(OUTLINE_THICKNESS);
	rectangle.setOutlineColor(sf::Color::White);
	convex.setOutlineColor(sf::Color::White);
	circle.setOutlineColor(sf::Color::White);
	//LuaScript script("data/setting.lua");
	//std::string fontPath = script.get<std::string>("FontPath");
	//font.loadFromFile(fontPath);
	info.setFont(font);
	info.setStyle(sf::Text::Regular);
	info.setFillColor(sf::Color::White);
	info.setCharacterSize(CHARACTER_SIZE);
}
Collider::~Collider()
{
	__DEBUG_EXEC(std::cout << "~Collider()\n");
}
const sf::CircleShape& Collider::getCircle() const 
{
	return circle;
}
const sf::RectangleShape& Collider::getRect() const
{
	return rectangle;
}
const sf::ConvexShape& Collider::getConvex() const
{
	return convex;
}
const sf::Text& Collider::getInfo() const
{
	return info;
}
void Collider::infoUpdate(float x, float y)
{
	info.setPosition(x, y - 2 * (2 * info.getLineSpacing() + info.getCharacterSize()));
	std::stringstream ss;
	ss << "X:" << x << "\nY:" << y;
	std::string infostr = ss.str();
	info.setString(infostr);
}
void Collider::infoUpdate(const sf::Vector2f& vec)
{
	info.setPosition(vec.x, vec.y - 2 * (2 * info.getLineSpacing() + info.getCharacterSize()));
	std::stringstream ss;
	ss << "X:" << vec.x << "\nY:" << vec.y;
	std::string infostr = ss.str();
	info.setString(infostr);
}
void Collider::setRectPosition(float x, float y)
{
	rectangle.setPosition(x, y);
	infoUpdate(x, y);
}
void Collider::setRectPosition(const sf::Vector2f& position)
{
	rectangle.setPosition(position);
	infoUpdate(position);
}
void Collider::setRectSize(const sf::Vector2f& size)
{
	rectangle.setSize(size);
}
void Collider::setConvexPointCount(std::size_t count)
{
	convex.setPointCount(count);
}
void Collider::setConvexPoint(std::size_t index, const sf::Vector2f& point)
{
	convex.setPoint(index, point);
}
void Collider::setOrigin(float x, float y)
{
	rectangle.setOrigin(x, y);
	convex.setOrigin(x, y);
	info.setOrigin(x, y);
}
void Collider::setOrigin(sf::Vector2f vec)
{
	rectangle.setOrigin(vec);
	convex.setOrigin(vec);
	info.setOrigin(vec);
}
void Collider::setRotation(float angle)
{
	rectangle.setRotation(angle);
	convex.setRotation(angle);
	info.setRotation(angle);
}
void Collider::setCircleRadius(float radius)
{
	circle.setRadius(radius);
}
void Collider::move(float x, float y)
{
	rectangle.move(x, y);
	convex.move(x, y);
	info.move(x, y);
	infoUpdate(rectangle.getPosition());
	circle.move(x, y);
}
void Collider::move(const sf::Vector2f& vec)
{
	rectangle.move(vec);
	convex.move(vec);
	info.move(vec);
	infoUpdate(rectangle.getPosition());
	circle.move(vec);
}
void Collider::setConvexPosition(float x, float y)
{
	convex.setPosition(x, y);
}
void Collider::setConvexPosition(const sf::Vector2f& vec)
{
	convex.setPosition(vec);
}
void Collider::setCirclePosition(float x, float y) 
{
	circle.setPosition(x, y);
}
void Collider::setCirclePosition(const sf::Vector2f& vec)
{
	circle.setPosition(vec);
}
void Collider::setCircleCenter(float x, float y)
{
	auto radius = circle.getRadius();
	circle.setPosition(x - radius, y - radius);
}
void Collider::setCircleCenter(const sf::Vector2f& vec)
{
	auto radius = circle.getRadius();
	circle.setPosition(vec.x - radius, vec.y - radius);
}
void Collider::setPosition(float x, float y)
{
	convex.setPosition(x, y);
	rectangle.setPosition(x, y);
	circle.setPosition(x, y);
}
void Collider::setPosition(const sf::Vector2f& vec)
{
	convex.setPosition(vec);
	rectangle.setPosition(vec);
	circle.setPosition(vec);
}
void Collider::setRect(const sf::FloatRect& fRect)
{
	sf::Vector2f size(fRect.width, fRect.height);
	setRectSize(size);
	setConvexPoint(0, sf::Vector2f(0, 0));
	setConvexPoint(1, sf::Vector2f(0 + fRect.width, 0));
	setConvexPoint(2, sf::Vector2f(0 + fRect.width, 0 + fRect.height));
	setConvexPoint(3, sf::Vector2f(0, 0 + fRect.height));
}
void Collider::setRect(const sf::IntRect& iRect)
{
	sf::Vector2f size(iRect.width, iRect.height);
	setRectSize(size);
	setConvexPoint(0, sf::Vector2f(0, 0));
	setConvexPoint(1, sf::Vector2f(0 + iRect.width, 0));
	setConvexPoint(2, sf::Vector2f(0 + iRect.width, 0 + iRect.height));
	setConvexPoint(3, sf::Vector2f(0, 0 + iRect.height));
}
Collision::Collision()
{
	__DEBUG_EXEC(std::cout << "Collision()\n");
}
Collision::~Collision()
{
	__DEBUG_EXEC(std::cout << "~Collision()\n");
}
bool Collision::intersect(const sf::CircleShape& circle1, const sf::CircleShape& circle2, sf::Vector2f* mtv_ptr)
{
	sf::Vector2f centre1 = getGlobalCenter(circle1);
	sf::Vector2f centre2 = getGlobalCenter(circle2);
	auto radius1 = circle1.getRadius();
	auto radius2 = circle2.getRadius();
	if (radius1 == 0 || radius2 == 0)
		return false;
	if (sqr(centre1.x - centre2.x) + sqr(centre1.y - centre2.y) > sqr(radius1 + radius2)) {
		if (mtv_ptr)
			*mtv_ptr = sf::Vector2f();
		return false;
	}
	if (mtv_ptr)
		*mtv_ptr = getNormalized(centre1 - centre2) * (radius1 + radius2 - getLength(centre1- centre2));
	return true;
}
bool Collision::intersect(const sf::RectangleShape& rectangle1, const sf::RectangleShape& rectangle2, sf::Vector2f* mtv_ptr)
{
	sf::FloatRect r1 = rectangle1.getLocalBounds();
	sf::FloatRect r2 = rectangle2.getLocalBounds();
	return r1.intersects(r2);
}
bool Collision::intersect(const sf::ConvexShape& convex1, const sf::ConvexShape& convex2, sf::Vector2f* mtv_ptr)
{
	std::vector<sf::Vector2f> vertices1 = getVertices(convex1);
	std::vector<sf::Vector2f> vertices2 = getVertices(convex2);
	std::vector<sf::Vector2f> axes = getPerpendicularAxes(vertices1, vertices2);
	float minOverlap = std::numeric_limits<float>::infinity();
	for (auto& axis : axes) {
		sf::Vector2f proj1 = projectOnAxis(vertices1, axis);
		sf::Vector2f proj2 = projectOnAxis(vertices2, axis);
		float overlap = getOverlapLength(proj1, proj2);
		if (overlap == 0.f) { 
			if (mtv_ptr)
				*mtv_ptr = sf::Vector2f();
			return false;
		} else {
			if (overlap < minOverlap) {
				minOverlap = overlap;
				if (mtv_ptr)
					*mtv_ptr = axis * minOverlap; 
			}
		}
	}
	if (mtv_ptr) {
		bool notPointingInTheSameDirection = dotProduct(getGlobalCenter(convex1) - getGlobalCenter(convex2), *mtv_ptr) < 0;
		if (notPointingInTheSameDirection) { (*mtv_ptr) = -(*mtv_ptr); }
	}
	return true;
}
bool Collision::isCollide(const Collider& collider1, const Collider& collider2, int collideMode, sf::Vector2f* mtv_ptr)
{
	switch (collideMode) {
	case CIRCLE_MODE:
		return intersect(collider1.getCircle(), collider2.getCircle(), mtv_ptr);	
	case RECT_MODE:
		return intersect(collider1.getRect(), collider2.getRect(), mtv_ptr);
	case CONVEX_MODE:
		return intersect(collider1.getConvex(), collider2.getConvex(), mtv_ptr);
	default:
		std::cerr << "_____ERROR: Unknown colliderMode " << collideMode << std::endl;
		return false;
	}
}