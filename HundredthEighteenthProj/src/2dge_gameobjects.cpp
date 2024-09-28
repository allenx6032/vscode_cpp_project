#include "2dge_gameobjects.hpp"
static const float SLOW_RATE = 0.95;
static const float SLOW_LIMIT = 50;
SpriteObject::SpriteObject():
	type(0)
{
	__DEBUG_EXEC(std::cout << "SpriteObject()\n");
}
SpriteObject::SpriteObject(const SpriteObject& obj):
	type(obj.type)
{
	__DEBUG_EXEC(std::cout << "SpriteObject(copy)\n");
	sprite.setTexture(*obj.sprite.getTexture());
	sprite.setTextureRect(obj.sprite.getTextureRect());
	sprite.setScale(obj.sprite.getScale());
}
SpriteObject::SpriteObject(SpriteObject&& obj):
	type(obj.type)
{
	__DEBUG_EXEC(std::cout << "SpriteObject(move)\n");
	sprite.setTexture(*obj.sprite.getTexture());
	sprite.setTextureRect(obj.sprite.getTextureRect());
	sprite.setScale(obj.sprite.getScale());
}
SpriteObject& SpriteObject::operator=(const SpriteObject& obj)
{
	type = obj.type;
	sprite.setTexture(*obj.sprite.getTexture());
	sprite.setTextureRect(obj.sprite.getTextureRect());
	sprite.setScale(obj.sprite.getScale());
	return *this;
}
SpriteObject& SpriteObject::operator=(SpriteObject&& obj)
{
	type = obj.type;
	sprite.setTexture(*obj.sprite.getTexture());
	sprite.setTextureRect(obj.sprite.getTextureRect());
	sprite.setScale(obj.sprite.getScale());
	return *this;
}
SpriteObject::~SpriteObject()
{
	__DEBUG_EXEC(std::cout << "~SpriteObject\n");
}
std::size_t SpriteObject::getType()
{
	return type;
}
void SpriteObject::setType(std::size_t type_)
{
	type = type_;
}
void SpriteObject::setTexture(std::shared_ptr<sf::Texture> texture_ptr)
{
	sprite.setTexture(*texture_ptr);
}
GameObject::GameObject():
	type(0),
	currentAnimationNum(0),
	isActive_(true),
	debugMode(false)
{
	collider.setConvexPointCount(4);
}
GameObject::GameObject(const GameObject& obj):
	type(obj.type),
	currentAnimationNum(obj.currentAnimationNum),
	isActive_(true),
	debugMode(obj.debugMode)
{
	collider.setConvexPointCount(4);
	collider.setCircleRadius(obj.collider.getCircle().getRadius());
	animationVec = obj.animationVec;
	aManager = obj.aManager;
	auto boundybox = obj.collider.getConvex();
	collider.setConvexPointCount(boundybox.getPointCount());
	for (std::size_t i = 0; i < boundybox.getPointCount(); i++) {
		collider.setConvexPoint(i, boundybox.getPoint(i));
	}
	setAnimation(currentAnimationNum);
}
GameObject::GameObject(GameObject&& obj):
	type(obj.type),
	currentAnimationNum(obj.currentAnimationNum),
	isActive_(true),
	debugMode(obj.debugMode)
{
	collider.setConvexPointCount(4);
	collider.setCircleRadius(obj.collider.getCircle().getRadius());
	animationVec = std::move(obj.animationVec);
	aManager = std::move(obj.aManager);
	auto boundybox = obj.collider.getConvex();
	collider.setConvexPointCount(boundybox.getPointCount());
	for (std::size_t i = 0; i < boundybox.getPointCount(); i++)
		collider.setConvexPoint(i, boundybox.getPoint(i));
	setAnimation(currentAnimationNum);
}
GameObject& GameObject::operator=(const GameObject& obj)
{
	collider.setConvexPointCount(4);
	collider.setCircleRadius(obj.collider.getCircle().getRadius());
	type = obj.type; 
	currentAnimationNum = obj.currentAnimationNum;
	isActive_ = true;
	animationVec = obj.animationVec;
	aManager = obj.aManager;
	auto boundybox = obj.collider.getConvex();
	collider.setConvexPointCount(boundybox.getPointCount());
	for (std::size_t i = 0; i < boundybox.getPointCount(); i++)
		collider.setConvexPoint(i, boundybox.getPoint(i));
	setAnimation(currentAnimationNum);
	debugMode = obj.isDebug();
	return *this;
}
GameObject& GameObject::operator=(GameObject&& obj)
{
	collider.setConvexPointCount(4);
	collider.setCircleRadius(obj.collider.getCircle().getRadius());
	type = obj.type; 
	currentAnimationNum = obj.currentAnimationNum;
	isActive_ = true;
	animationVec = std::move(obj.animationVec);
	aManager = std::move(obj.aManager);
	auto boundybox = obj.collider.getConvex();
	collider.setConvexPointCount(boundybox.getPointCount());
	for (std::size_t i = 0; i < boundybox.getPointCount(); i++)
		collider.setConvexPoint(i, boundybox.getPoint(i));	
	setAnimation(currentAnimationNum);
	debugMode = obj.isDebug();
	return *this;
}
void GameObject::addAnimation(std::shared_ptr<Animation> animation_ptr, std::size_t num) 
{
	assert(animation_ptr);
	assert(num < AnimationTypeSize);
	animationVec[num] = animation_ptr;
	//sf::IntRect iRect = animation_ptr->getFrame(0);
	//collider.setRect(iRect);
}
bool GameObject::setAnimation(std::size_t num)
{
	assert(animationVec[num]);
	currentAnimationNum = num;
	aManager.setAnimation(*animationVec[num].get());
	return true;
}
bool GameObject::playAnimation(std::size_t num, bool next)
{
	assert(animationVec[num]);
	if ((currentAnimationNum == AnimationType::attack && aManager.isPaused())
	|| currentAnimationNum != AnimationType::attack) {
		currentAnimationNum = num;
		aManager.play(*animationVec[num].get(), next);
		return true;
	}
	return false;
}
const Animation* GameObject::getAnimation(std::size_t num) const 
{
	if (num < AnimationTypeSize)
		return animationVec[num].get();
	return nullptr;
}
std::size_t GameObject::getCurrentAnimationNum() const
{
	return currentAnimationNum;
}
void GameObject::render(sf::RenderTarget& target, sf::Time frameTime)
{
	aManager.play();
	aManager.update(frameTime);
	target.draw(aManager);
	if (debugMode) {
		target.draw(collider.getInfo());
		target.draw(collider.getConvex());
		target.draw(collider.getCircle());
	}
}
void GameObject::setPosition(float x, float y)
{
	aManager.setPosition(x, y);
	collider.setPosition(x, y);
	auto rect = aManager.getLocalBounds();
	collider.setCircleCenter(x + rect.width / 2, y + rect.height / 2);
}
void GameObject::setPosition(const sf::Vector2f& position)
{
	aManager.setPosition(position);
	collider.setPosition(position);
	auto rect = aManager.getLocalBounds();
	collider.setCircleCenter(position.x + rect.width / 2, position.y + rect.height / 2);
}
void GameObject::setCenter(float x, float y)
{
	auto rect = aManager.getLocalBounds();
	auto pos =  sf::Vector2f(x - rect.width / 2, y - rect.height / 2);
	aManager.setPosition(pos);
	collider.setPosition(pos);
	collider.setCircleCenter(x, y);
}
void GameObject::setCenter(const sf::Vector2f& position)
{
	auto rect = aManager.getLocalBounds();
	auto pos =  sf::Vector2f(position.x - rect.width / 2, position.y - rect.height / 2);
	aManager.setPosition(pos);
	collider.setPosition(pos);
	collider.setCircleCenter(position);
}
sf::Vector2f GameObject::getPosition() const 
{
	return aManager.getPosition();
}
void GameObject::setRotation(float angle)
{
	aManager.setRotation(angle);
	collider.setRotation(angle);
}
void GameObject::setRotationAroundCentre(float angle)
{
	auto oldCentre = getGlobalCenter();
	setRotation(angle);
	auto newCentre = getGlobalCenter();
	move(oldCentre - newCentre); 
}
void GameObject::move(const sf::Vector2f& vec)
{
	aManager.move(vec);
	collider.move(vec);
}
const Collider& GameObject::getCollider() const
{
	return collider;
}
std::size_t GameObject::getType() const
{
	return type;
}
void GameObject::setType(std::size_t type_)
{
	type = type_;
}
GameObject& GameObject::getRef()
{
	return std::ref(*this);
}
void GameObject::activate()
{
	isActive_ = true;
}
void GameObject::deactivate()
{
	isActive_ = false;
}
bool GameObject::isActive() const 
{
	return isActive_;
}
void GameObject::setOrigin(float x, float y)
{
	aManager.setOrigin(x, y);
	collider.setOrigin(x, y);
}
void GameObject::setOrigin(const sf::Vector2f& vec)
{
	aManager.setOrigin(vec);
	collider.setOrigin(vec);
}
bool GameObject::isDebug() const 
{
	return debugMode;
}
void GameObject::scale(float x, float y)
{
	aManager.scale(x, y);
	auto convex = collider.getConvex(); 
	for (std::size_t i = 0; i < convex.getPointCount(); i++) {
		auto point = convex.getPoint(i);
		collider.setConvexPoint(i, sf::Vector2f(point.x * x, point.y * y));
	}
}
void GameObject::setScale(float x, float y)
{
	aManager.setScale(x, y);
	auto convex = collider.getConvex();
	for (std::size_t i = 0; i < convex.getPointCount(); i++) {
		auto point = convex.getPoint(i);
		collider.setConvexPoint(i, sf::Vector2f(point.x * x, point.y * y));
	}
}
void GameObject::mirrorFlip()
{
	aManager.setOrigin(aManager.getLocalBounds().width, 0);
	aManager.setScale(-1, 1);
}
void GameObject::mirrorUnFlip()
{
	aManager.setOrigin(0, 0);
	aManager.setScale(1, 1);
}
void GameObject::setBoundyBox(const std::vector<sf::Vector2f>& vertices)
{
	auto size = vertices.size();
	assert(size > 2);
	collider.setConvexPointCount(size);
	for (std::size_t i = 0; i < size; i++) {
		collider.setConvexPoint(i, vertices[i]);
	}
}
void GameObject::setBoundyBoxVertex(const sf::Vector2f& vertex, std::size_t index)
{
	collider.setConvexPoint(index, vertex);
}
void GameObject::setBoundyBoxVertex(float x, float y, std::size_t index)
{
	collider.setConvexPoint(index, sf::Vector2f(x, y));
}
void GameObject::setInteractRadius(float radius)
{
	collider.setCircleRadius(radius);
}
void GameObject::setLooped(bool looped)
{
	aManager.setLooped(looped);
}
bool GameObject::isMirror() const
{
	if (aManager.getScale().x > 0)
		return false;
	return true;
}
std::size_t GameObject::getCurrentAnimationFrame() const 
{
	return aManager.getCurrentFrame();
}
sf::Vector2f GameObject::getGlobalCenter() const
{
	const sf::Transform& transform = aManager.getTransform();
	sf::FloatRect local = aManager.getLocalBounds();
	return transform.transformPoint(local.left + local.width / 2.f, local.top + local.height / 2.f);
}
sf::Vector2f GameObject::getLocalCenter() const
{
	sf::FloatRect local = aManager.getLocalBounds();
	return sf::Vector2f(local.left + local.width / 2.f, local.top + local.height / 2.f);
}
void GameObject::reset()
{
	setRotation(0);
	setPosition(0 ,0);
	setAnimation(0);
}
StaticGameObject::StaticGameObject()
{
	__DEBUG_EXEC(std::cout << "StaticGameObject()\n");
}
StaticGameObject::StaticGameObject(const StaticGameObject& obj):
	GameObject(obj)
{
	__DEBUG_EXEC(std::cout << "StaticGameObject(copy)\n");
}
StaticGameObject::StaticGameObject(StaticGameObject&& obj):
	GameObject(obj)
{
	__DEBUG_EXEC(std::cout << "StaticGameObject(move)\n");;
}
StaticGameObject& StaticGameObject::operator=(const StaticGameObject& obj)
{
	collider.setConvexPointCount(4);
	setType(obj.getType());
	activate();
	animationVec = obj.animationVec;
	aManager = obj.aManager;
	auto boundybox = collider.getConvex();
	collider.setConvexPointCount(boundybox.getPointCount());
	for (std::size_t i = 0; i < boundybox.getPointCount(); i++)
		collider.setConvexPoint(i, boundybox.getPoint(i));
	return *this;
}
StaticGameObject& StaticGameObject::operator=(StaticGameObject&& obj)
{
	collider.setConvexPointCount(4);
	setType(obj.getType());
	activate();
	animationVec = std::move(obj.animationVec);
	aManager = std::move(obj.aManager);
	auto boundybox = collider.getConvex();
	collider.setConvexPointCount(boundybox.getPointCount());
	for (std::size_t i = 0; i < boundybox.getPointCount(); i++)
		collider.setConvexPoint(i, boundybox.getPoint(i));
	return *this;
}
StaticGameObject::~StaticGameObject()
{
	__DEBUG_EXEC(std::cout << "~StaticGameObject()\n");
}
StaticGameObject& StaticGameObject::getRef()
{
	return std::ref(*this);
}
DynamicGameObject::DynamicGameObject(sf::Vector2f velocity_):
	velocity(velocity_),
	activateTime(sf::Time::Zero),
	deactivateTime(sf::Time::Zero),
	slowmode(true),
	AIclass("nope"),
	ghostmode(false)
{
	__DEBUG_EXEC(std::cout << "DynamicGameObject(std::vector<float>)\n");
}
DynamicGameObject::DynamicGameObject(const DynamicGameObject& obj):
	GameObject(obj),
	ParameterObject(obj),
	velocity(obj.velocity),
	activateTime(obj.activateTime),
	deactivateTime(obj.deactivateTime),
	slowmode(obj.slowmode),
	AIclass(obj.AIclass),
	ghostmode(false)
{
	__DEBUG_EXEC(std::cout << "DynamicGameObject(copy)\n");
}
DynamicGameObject::DynamicGameObject(DynamicGameObject&& obj):
	GameObject(obj),
	ParameterObject(obj),
	activateTime(obj.activateTime),
	deactivateTime(obj.deactivateTime),
	slowmode(obj.slowmode),
	AIclass(obj.AIclass),
	ghostmode(false)
{
	__DEBUG_EXEC(std::cout << "DynamicGameObject(move)\n");
	velocity = std::move(obj.velocity);
}
DynamicGameObject& DynamicGameObject::operator=(const DynamicGameObject& obj)
{
	setHp(obj.getHp());
	setMaxHp(obj.getMaxHp());
	setDamage(obj.getDamage());
	collider.setConvexPointCount(4);
	setType(obj.getType());
	activate();
	setSlowMode(obj.getSlowMode());
	animationVec = obj.animationVec;
	aManager = obj.aManager;
	AIclass = obj.AIclass;
	activateTime = obj.activateTime;
	deactivateTime = obj.deactivateTime;
	auto boundybox = obj.collider.getConvex();
	collider.setConvexPointCount(boundybox.getPointCount());
	for (std::size_t i = 0; i < boundybox.getPointCount(); i++)
		collider.setConvexPoint(i, boundybox.getPoint(i));
	return *this;
}
DynamicGameObject& DynamicGameObject::operator=(DynamicGameObject&& obj)
{
	setHp(obj.getHp());
	setMaxHp(obj.getMaxHp());
	setDamage(obj.getDamage());
	collider.setConvexPointCount(4);
	setType(obj.getType());
	activate();
	setSlowMode(obj.getSlowMode());
	animationVec = std::move(obj.animationVec);
	aManager = std::move(obj.aManager);
	AIclass = obj.AIclass;
	activateTime = obj.activateTime;
	deactivateTime = obj.deactivateTime;
	auto boundybox = obj.collider.getConvex();
	collider.setConvexPointCount(boundybox.getPointCount());
	for (std::size_t i = 0; i < boundybox.getPointCount(); i++)
		collider.setConvexPoint(i, boundybox.getPoint(i));
	return *this;
}
DynamicGameObject::~DynamicGameObject()
{
	__DEBUG_EXEC(std::cout << "~DynamicGameObject()\n");
}

void DynamicGameObject::setVelocity(const sf::Vector2f& velocity_)
{
	if (getCurrentAnimationNum() != AnimationType::attack)
		velocity = velocity_;
}
void DynamicGameObject::setVelocity(float x, float y)
{
	if (getCurrentAnimationNum() != AnimationType::attack)
		velocity = sf::Vector2f(x, y);
}
sf::Vector2f DynamicGameObject::getVelocity() const
{
	return velocity;
}
void DynamicGameObject::setSlowMode(bool slowmode_)
{
	slowmode = slowmode_;
}
bool DynamicGameObject::getSlowMode() const
{
	return slowmode;
}
void DynamicGameObject::update()
{
	float velx = velocity.x;
	float vely = velocity.y;
	if (slowmode) {
		if (velx != 0) {
			if (abs(velx) > SLOW_LIMIT)
				velocity.x *= SLOW_RATE;
			else
				velocity.x = 0;
		}
		if (vely != 0) {
			if (abs(vely) > SLOW_LIMIT)
				velocity.y *= SLOW_RATE;
			else
				velocity.y = 0;
		}
	}
	if (velx == 0 && vely == 0 && getCurrentAnimationNum() != AnimationType::death) {
		playAnimation(AnimationType::idle, true);
	}
	aManager.move(velocity.x * deltaTime.asSeconds(), velocity.y * deltaTime.asSeconds());
	collider.move(velocity.x * deltaTime.asSeconds(), velocity.y * deltaTime.asSeconds());
}
void DynamicGameObject::render(sf::RenderTarget& target, sf::Time frameTime)
{
	//if (velocity.x != 0 || velocity.y != 0)
		//aManager.play();
	//else
		//aManager.stop();
	aManager.update(frameTime);
	target.draw(aManager);
	if (isDebug()) {
		target.draw(collider.getInfo());
		target.draw(collider.getConvex());
		target.draw(collider.getCircle());
	}
}
DynamicGameObject& DynamicGameObject::getRef()
{
	return std::ref(*this);
}
void DynamicGameObject::setActivateTime(sf::Time time)
{
	activateTime = time;
}
sf::Time DynamicGameObject::getActivateTime() const 
{
	return activateTime;
}
sf::Time DynamicGameObject::getElapsedTime() const
{
	return clock.getElapsedTime();
}
sf::Time DynamicGameObject::restartTime()
{
	return clock.restart();
}
void DynamicGameObject::reset()
{
	GameObject::reset();
	clock.restart();
	setHp(getMaxHp());
	setState("idle");
	setGhostMode(false);
}
void DynamicGameObject::setDeactivateTime(sf::Time time)
{
	deactivateTime = time;
}
sf::Time DynamicGameObject::getDeactivateTime() const
{
	return deactivateTime;
}
void DynamicGameObject::clockRestart()
{
	clock.restart();
}
void DynamicGameObject::setAIclass(std::string ai)
{
	AIclass = ai;
}
const std::string& DynamicGameObject::getAIclass() const
{
	return AIclass;
}
void DynamicGameObject::setGhostMode(bool mode)
{
	ghostmode = mode;
}
bool DynamicGameObject::getGhostMode() const
{
	return ghostmode;
}
ParameterObject::ParameterObject(int hp):
_hp(hp),
_maxhp(hp),
_damage(0),
_state("idle")
{
	__DEBUG_EXEC(std::cout << "ParameterObject()\n");
}
ParameterObject::ParameterObject(const ParameterObject& obj):
_hp(obj._hp),
_maxhp(obj._maxhp),
_damage(obj._damage),
_state("idle"),
_target(0, 0)
{
	__DEBUG_EXEC(std::cout << "ParameterObject(copy)\n");
}
ParameterObject::ParameterObject(ParameterObject&& obj):
_hp(obj._hp),
_maxhp(obj._maxhp),
_damage(obj._damage),
_state("idle"),
_target(0, 0)
{
	__DEBUG_EXEC(std::cout << "ParameterObject(move)\n");
}
ParameterObject& ParameterObject::operator=(const ParameterObject& obj)
{
	//later
}
ParameterObject& ParameterObject::operator=(ParameterObject&& obj)
{
	//later
}
void ParameterObject::setHp(int hp)
{
	_hp = hp;
}
void ParameterObject::setMaxHp(int hp)
{
	_maxhp = hp;
}
void ParameterObject::setDamage(int damage)
{
	_damage = damage;
}
void ParameterObject::setState(const std::string& newState)
{
	_state = newState;
}
void ParameterObject::setTarget(const sf::Vector2f& target)
{
	_target = target;
}
int ParameterObject::getHp() const
{
	return _hp;
}
int ParameterObject::getMaxHp() const
{
	return _maxhp;
}
int ParameterObject::getDamage() const
{
	return _damage;
}
std::string ParameterObject::getState() const
{
	return _state;
}
sf::Vector2f ParameterObject::getTarget() const
{
	return _target;	
}