#ifndef GAMEOBJECT_HPP_
#define GAMEOBJECT_HPP_
#include <SFML_Graphics.hpp>
#include <vector>
#include <memory>
#include "2dge_animation.hpp"
#include "2dge_collision.hpp"
#include "2dge_interaction.hpp"
extern const sf::Time deltaTime;
#define NDEBUG
#ifndef NDEBUG
#define __DEBUG_EXEC(code) code
#else
#define __DEBUG_EXEC(code) ;
#endif
enum AnimationType {
	idle = 0,
	walk,
	attack,
	death,
	AnimationTypeSize
};
class SpriteObject
{
	std::size_t type;
public:
	sf::Sprite sprite;
	SpriteObject();
	SpriteObject(const SpriteObject& obj);
	SpriteObject(SpriteObject&& obj);
	SpriteObject& operator=(const SpriteObject& obj);
	SpriteObject& operator=(SpriteObject&& obj);
	~SpriteObject();
	std::size_t getType();
	void setType(std::size_t type);
	void setTexture(std::shared_ptr<sf::Texture> texture_ptr);
};
class GameObject
{
	std::size_t type;
	std::size_t currentAnimationNum;
	bool isActive_;
	bool debugMode;
protected:
	std::vector<std::shared_ptr<const Animation>> animationVec = std::vector<std::shared_ptr<const Animation>>(AnimationTypeSize);
	Collider collider;
	AnimationManager aManager;
public:
	GameObject();
	GameObject(const GameObject& obj);
	GameObject(GameObject&& obj);
	GameObject& operator=(const GameObject& obj);
	GameObject& operator=(GameObject&& obj);
	virtual ~GameObject() {}
	void addAnimation(std::shared_ptr<Animation> animation_ptr, std::size_t num);
	bool setAnimation(std::size_t num);
	bool playAnimation(std::size_t num, bool next = false);
	const Animation* getAnimation(std::size_t num) const;
	std::size_t getCurrentAnimationNum() const;
	void setPosition(float x, float y);
	void setPosition(const sf::Vector2f& position);
	void setCenter(float x, float y);
	void setCenter(const sf::Vector2f& position);
	sf::Vector2f getPosition() const;
	void setRotation(float angle);
	void setRotationAroundCentre(float angle);
	void setOrigin(float x, float y);
	void setOrigin(const sf::Vector2f& vec);
	void move(const sf::Vector2f& vec);
	void scale(float x, float y);
	void setScale(float x, float y);
	void mirrorFlip();
	void mirrorUnFlip();
	bool isMirror() const;
	void setBoundyBox(const std::vector<sf::Vector2f>& vertices);
	void setBoundyBoxVertex(const sf::Vector2f& vertex, std::size_t index);
	void setBoundyBoxVertex(float x, float y, std::size_t index);
	void setInteractRadius(float radius);
	void setLooped(bool looped);
	virtual void render(sf::RenderTarget& target, sf::Time frameTime);
	const Collider& getCollider() const;
	std::size_t getType() const;
	void setType(std::size_t type);
	void activate();
	void deactivate();
	bool isActive() const;
	bool isDebug() const;
	virtual GameObject& getRef();
	std::size_t getCurrentAnimationFrame() const;
	sf::Vector2f getGlobalCenter() const;
	sf::Vector2f getLocalCenter() const;
	virtual void reset();
};
class ParameterObject
{
	int _hp;
	int _maxhp;
	int _damage;
	std::string _state;
	sf::Vector2f _target;
public:
	ParameterObject(int hp = 0);
	ParameterObject(const ParameterObject& obj);
	ParameterObject(ParameterObject&& obj);
	ParameterObject& operator=(const ParameterObject& obj);
	ParameterObject& operator=(ParameterObject&& obj);
	void setHp(int hp);
	void setMaxHp(int hp);
	void setDamage(int damage);
	void setState(const std::string& newState);
	void setTarget(const sf::Vector2f& target);
	int getHp() const;
	int getMaxHp() const;
	int getDamage() const;
	std::string getState() const;
	sf::Vector2f getTarget() const;
};
class StaticGameObject : public GameObject
{
public:
	StaticGameObject();
	StaticGameObject(const StaticGameObject& obj);
	StaticGameObject(StaticGameObject&& obj);
	StaticGameObject& operator=(const StaticGameObject& obj);
	StaticGameObject& operator=(StaticGameObject&& obj);
	~StaticGameObject();
	virtual StaticGameObject& getRef();
};
class DynamicGameObject : public GameObject, public ParameterObject
{
	sf::Vector2f velocity;
	sf::Clock clock;
	sf::Time activateTime;
	sf::Time deactivateTime;
	bool slowmode;
	std::string AIclass;
	bool ghostmode;
public:
	DynamicGameObject(sf::Vector2f velocity_ = {0, 0});
	DynamicGameObject(const DynamicGameObject& obj);
	DynamicGameObject(DynamicGameObject&& obj);
	DynamicGameObject& operator=(const DynamicGameObject& obj);
	DynamicGameObject& operator=(DynamicGameObject&& obj);
	~DynamicGameObject();
	void setVelocity(const sf::Vector2f& velocity_);
	void setVelocity(float x, float y);
	void setSlowMode(bool slowmode);
	void setActivateTime(sf::Time time);
	void setDeactivateTime(sf::Time time);
	void setAIclass(std::string ai);
	void clockRestart();
	void setGhostMode(bool mode);
	sf::Time restartTime();
	sf::Time getActivateTime() const;
	sf::Time getDeactivateTime() const;
	sf::Time getElapsedTime() const;
	bool getSlowMode() const;
	bool getGhostMode() const;
	sf::Vector2f getVelocity() const;
	const std::string& getAIclass() const;
	virtual void render(sf::RenderTarget& target, sf::Time frameTime);
	virtual void update();
	virtual void reset();
	virtual DynamicGameObject& getRef();
};

#endif //GAMEOBJECT_HPP_