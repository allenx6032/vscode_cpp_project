#pragma once

#include <string>
#include <SDL.h>

/*
* \param const std::string& assetId = ""
	\param unsigned int width = 32
	\param unsigned int height = 32
	\param  unsigned int zIndex = 0
	\param bool isCameraIndependent = false
	\param Uint16 tilemapXCoord = 0
	\param Uint16 tilemapYCoord = 0
*/
struct SpriteComponent
{
	std::string m_assetId;
	unsigned int m_width;
	unsigned int m_height;
	unsigned int m_zIndex;
	SDL_RendererFlip m_flip;
	bool m_isInCameraSpace;
	// this rect allows the texture to come from a tilemaps 
	SDL_Rect m_textureRect;
	

	SpriteComponent(const std::string& assetId = "", unsigned int width = 32, unsigned int height = 32, unsigned int zIndex = 0, bool isCameraIndependent = false, Uint16 tilemapXCoord = 0, Uint16 tilemapYCoord = 0)
		: m_assetId(assetId)
		, m_width(width)
		, m_height(height)
		, m_zIndex(zIndex)
		, m_flip(SDL_FLIP_NONE)
		, m_isInCameraSpace(isCameraIndependent)
		, m_textureRect(SDL_Rect{
			static_cast<int>(tilemapXCoord), 
			static_cast<int>(tilemapYCoord), 
			static_cast<int>(width),
			static_cast<int>(height)
			})
	{
	}
};