#pragma once

#include <string>
#include <glm/glm.hpp>
#include <SDL.h>

/**
	\param glm::vec2 position = glm::vec2(0, 0), 
	\param std::string text = "default text for label", 
	\param std::string assetName = "charriot-font", 
	\param const SDL_Color& textColor = {255, 0, 255, 255}
	\param bool m_isInCameraSpace = true
*/
struct TextLabelComponent
{
	TextLabelComponent(glm::vec2 position = glm::vec2(0, 0), std::string text = "default text for label", std::string assetName = "charriot-font", const SDL_Color& textColor = {255, 0, 255, 255}, bool isInCameraSpace = true)
		: m_position(position)
		, m_text(text)
		, m_assetName(assetName)
		, m_textColor(textColor)
		, m_isInCameraSpace(isInCameraSpace)
	{
	}

	glm::vec2 m_position;
	std::string m_text;
	std::string m_assetName;
	SDL_Color m_textColor;
	bool m_isInCameraSpace;
};