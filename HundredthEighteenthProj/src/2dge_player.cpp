#include "2dge_player.hpp"
static const float SPEED_RATE = 200;
static const double PI = 3.1415926535;
Player::Player(ObjectManager& oManager_):
oManager(oManager_),
shootfreq(sf::milliseconds(250))
{
	__DEBUG_EXEC(std::cout << "Player()\n");
}
Player::~Player()
{
	__DEBUG_EXEC(std::cout << "~Player()\n");
}
void Player::setObject(DynamicGameObject* obj_ptr_)
{
	assert(obj_ptr_);
	obj_ptr = std::shared_ptr<DynamicGameObject>(obj_ptr_);
}
void Player::setObject(std::shared_ptr<DynamicGameObject> obj_ptr_)
{
	assert(obj_ptr_);
	obj_ptr = obj_ptr_;
}
std::shared_ptr<DynamicGameObject> Player::getObject()
{
	return obj_ptr;
}
void Player::shoot(sf::RenderWindow& window)
{
	;
}
sf::Vector2f Player::getPosition() const
{
	return obj_ptr->getPosition();
}