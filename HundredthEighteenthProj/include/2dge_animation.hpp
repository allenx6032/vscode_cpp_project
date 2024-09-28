#ifndef ANIMATION_HPP_
#define ANIMATION_HPP_
#include <SFML_Graphics.hpp>
#include <iostream>
#include <vector>
#include <memory>
#include <cassert>
#define NDEBUG
#ifndef NDEBUG
#define __DEBUG_EXEC(code) code
#else
#define __DEBUG_EXEC(code) ;
#endif
class Animation
{
	std::vector<sf::IntRect> Frames_;
	std::shared_ptr<const sf::Texture> texture_ptr;
public:
	Animation();
	Animation(const Animation& animation);
	Animation(Animation&& animation);
	Animation& operator=(const Animation& animation);
	Animation& operator=(Animation&& animation);
	~Animation();
	void addFrame(sf::IntRect rect);
	void setSpriteSheet(const sf::Texture* texture_ptr_);
	void setSpriteSheet(std::shared_ptr<sf::Texture> texture_ptr_);
	const sf::Texture* getSpriteSheet() const;
	std::size_t getSize() const;
	const sf::IntRect& getFrame(std::size_t num) const;
};
class AnimationManager : public sf::Drawable, public sf::Transformable
{
	const Animation* animation_;
	const sf::Texture* texture_;
	sf::Time frameTime_;
	sf::Time currentTime_;
	std::size_t currentFrame_;
	sf::Vertex vertices[4];
	bool isPaused_;
	bool isLooped_;
	bool isNextLoop_;
public:
	AnimationManager(sf::Time frameTime = sf::seconds(0.2f), bool Pause = false, bool Loop = false);
	AnimationManager(const AnimationManager& aManager);
	AnimationManager(AnimationManager&& aManager);
	AnimationManager& operator=(const AnimationManager& aManager);
	AnimationManager& operator=(AnimationManager&& aManager);
	~AnimationManager();
	void setAnimation(const Animation& animation);
	const Animation* getAnimation() const;
	void setFrameTime(sf::Time time);
	void update(sf::Time deltaTime);
	void play(bool next = false);
	void play(const Animation& animation, bool next = false);
	void pause();
	void stop();
	void setLooped(bool Looped);
	void setPaused(bool Paused);
	sf::FloatRect getLocalBounds() const;
	sf::FloatRect getGlobalBounds() const;
	bool isLooped() const;
	bool isPaused() const;
	sf::Time getFrameTime() const;
	void setFrame(std::size_t newFrame, bool resetTime = true);
	std::size_t getCurrentFrame() const;
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};
#endif //ANIMATION_HPP_