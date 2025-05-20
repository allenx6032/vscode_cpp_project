#pragma once

#include <map>
#include <string>
#include <SDL_ttf.h>

struct SDL_Renderer;
struct SDL_Texture;

class AssetStore
{
public:
	AssetStore();
	~AssetStore();

	void AddTexture(SDL_Renderer* renderer, const std::string& textureName, const std::string& filePath);
	SDL_Texture* GetTexture(const std::string& assetName) const;

	void AddFont(const std::string& fontName, const std::string& filePath, int fontSize);
	TTF_Font* GetFont(const std::string& fontName) ;

	void ClearAssets();
private:
	std::map<std::string, SDL_Texture*> m_textures;
	std::map<std::string, TTF_Font*> m_fonts;
};

