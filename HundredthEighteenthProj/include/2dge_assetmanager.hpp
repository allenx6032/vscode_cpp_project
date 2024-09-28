#ifndef ASSETMANAGER_HPP_
#define ASSETMANAGER_HPP_
#include <SFML_Graphics.hpp>
#include <vector>
#include <map>
#include <memory>
class AssetManager
{
	std::map<std::string, std::shared_ptr<sf::Texture>> tMap;
	std::map<std::string, std::shared_ptr<Animation>> aMap;
public:
	AssetManager();
	~AssetManager();
	template<typename T>
	std::shared_ptr<T> get(const std::string& key) {
		return nullptr;
	}
	template<typename... Args>
	std::shared_ptr<sf::Texture> createTexture(const std::string& key, Args... args);
	template<typename... Args>
	std::shared_ptr<Animation> createAnimation(const std::string& key, Args... args);
	template<typename T>
	void add(const std::string& key, T* value) {
		std::cerr << "AssetManager::add type error\n";
	}
	template<typename T>
	void add(const std::string& key, std::shared_ptr<T> value) {
		std::cerr << "AssetManager::add type error\n";
	}
};
template<>
inline std::shared_ptr<sf::Texture> AssetManager::get<sf::Texture>(const std::string& key)
{
	std::map<std::string, std::shared_ptr<sf::Texture>>::iterator it = tMap.find(key);
	if (it != tMap.end())
		return tMap[key];
	return nullptr;
}
template<>
inline void AssetManager::add<sf::Texture>(const std::string& key, sf::Texture* texture_ptr_)
{
	std::shared_ptr<sf::Texture> texture_ptr(texture_ptr_);
	if (!texture_ptr)
		fprintf(stderr, "AssetManager::add shared_ptr<sf::Texture> error");
	tMap.insert(std::pair<std::string, std::shared_ptr<sf::Texture>>(key, texture_ptr));
}
template<>
inline void AssetManager::add<sf::Texture>(const std::string& key, std::shared_ptr<sf::Texture> texture_ptr)
{
	if (!texture_ptr)
		fprintf(stderr, "AssetManager::add shared_ptr<sf::Texture> error");
	tMap.insert(std::pair<std::string, std::shared_ptr<sf::Texture>>(key, texture_ptr));
}
template<typename... Args>
inline std::shared_ptr<sf::Texture> AssetManager::createTexture(const std::string& key, Args... args)
{
	auto texture_ptr = std::make_shared<sf::Texture>(args...);
	if (!texture_ptr)
		fprintf(stderr, "AssetManager::create shared_ptr<sf::Texture> error");
	tMap.insert(std::pair<std::string, std::shared_ptr<sf::Texture>>(key, texture_ptr));
	return texture_ptr;
}
template<>
inline std::shared_ptr<Animation> AssetManager::get<Animation>(const std::string& key)
{
	std::map<std::string, std::shared_ptr<Animation>>::iterator it = aMap.find(key);
	if (it != aMap.end())
		return aMap[key];
	return nullptr;
}
template<>
inline void AssetManager::add<Animation>(const std::string& key, Animation* animation_ptr_)
{
	std::shared_ptr<Animation> animation_ptr(animation_ptr_);
	if (!animation_ptr)
		fprintf(stderr, "AssetManager::add shared_ptr<Animation> error");
	aMap.insert(std::pair<std::string, std::shared_ptr<Animation>>(key, animation_ptr));
}
template<>
inline void AssetManager::add<Animation>(const std::string& key, std::shared_ptr<Animation> animation_ptr)
{
	if (!animation_ptr)
		fprintf(stderr, "AssetManager::add shared_ptr<Animation> error");
	aMap.insert(std::pair<std::string, std::shared_ptr<Animation>>(key, animation_ptr));
}
template<typename... Args>
inline std::shared_ptr<Animation> AssetManager::createAnimation(const std::string& key, Args... args)
{
	auto animation_ptr = std::make_shared<Animation>(args...);
	if (!animation_ptr)
		fprintf(stderr, "AssetManager::create shared_ptr<Animation> error");
	aMap.insert(std::pair<std::string, std::shared_ptr<Animation>>(key, animation_ptr));
	return animation_ptr;
}
#endif //ASSETMANAGER_HPP_