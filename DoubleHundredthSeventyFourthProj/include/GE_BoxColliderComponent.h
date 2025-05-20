#pragma once

#include <glm/glm.hpp>

/*
* \param uint width = 32 - if specified on lua, default value will be scale.x * sprite.x
* \param uint height = 32 - if specified on lua, default value will be scale.y * sprite.y
* \param glm::vec2 offset = (0,0)
* \param bool isActive = true
*/
struct BoxColliderComponent
{
	unsigned int m_width;
	unsigned int m_height;
	glm::vec2 m_offset;
	bool m_isColliding;
	bool m_isActive;

	BoxColliderComponent(unsigned int width = 32, unsigned int height = 32, glm::vec2 offset = glm::vec2(0,0), bool isActive = true)
		: m_width(width)
		, m_height(height)
		, m_offset(offset)
		, m_isColliding(false)
		, m_isActive(isActive)
	{
	}
};