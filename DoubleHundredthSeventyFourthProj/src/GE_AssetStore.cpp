#include "GE_pch.h"

#include "GE_AssetStore.h"

#include "GE_Logger.h"

AssetStore::AssetStore()
{
    Logger::Log("AssetStore constructed");
}

AssetStore::~AssetStore()
{
    ClearAssets();
    Logger::Log("AssetStore destructed");
}

void AssetStore::AddTexture(SDL_Renderer* renderer, const std::string& assetName, const std::string& filePath)
{
    SDL_Surface* surface = IMG_Load(filePath.c_str());
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (texture)
    {
        m_textures.emplace(assetName, texture);
        Logger::Log("new texture added to AssetStore with id= " + assetName);
    }
    else
    {
        Logger::Error("null texture with name = " + assetName + " and filepath = " + filePath);
    }

}

SDL_Texture* AssetStore::GetTexture(const std::string& assetName) const
{
    return m_textures.at(assetName);
}

void AssetStore::AddFont(const std::string& fontName, const std::string& filePath, int fontSize)
{
    m_fonts.emplace(fontName, TTF_OpenFont(filePath.c_str(), fontSize));
}

TTF_Font* AssetStore::GetFont(const std::string& fontName) 
{
    return m_fonts[fontName];
}

void AssetStore::ClearAssets()
{
    for (auto texture : m_textures)
    {
        SDL_DestroyTexture(texture.second);
    }
    m_textures.clear(); 
    
    for (auto font : m_fonts)
    {
        TTF_CloseFont(font.second);
    }
    m_fonts.clear();
}
