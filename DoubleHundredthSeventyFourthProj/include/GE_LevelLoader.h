#pragma once

#include <memory>
#include <string>

class Registry;
class AssetStore;
struct SDL_Renderer;
namespace sol { class state; }

class LevelLoader
{
public:
	LevelLoader();
	~LevelLoader() = default;

	// just the name of the file - no paths or extensions
	void LoadLevel(const std::string& levelToLoad, const std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetStore, SDL_Renderer* renderer, sol::state& lua);
	void LoadLevel(unsigned int levelToLoad, const std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetStore, SDL_Renderer* renderer, sol::state& lua);
private:
	void ParseLevel(const std::string& levelToLoad, const std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetStore, SDL_Renderer* renderer, sol::state& lua);
};