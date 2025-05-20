#pragma once
#include <glm/glm.hpp>

/*
* \param glm::vec2 position = (0,0)
* \param glm::vec2 scale = (1,1)
* \param double m_rotation = 0
*/
struct TransformComponent
{
	glm::vec2 m_position;
	glm::vec2 m_scale;
	double m_rotation;

	TransformComponent(glm::vec2 position = glm::vec2(0,0), glm::vec2 scale = glm::vec2(1, 1), double rotation = 0)
		: m_position(position)
		, m_scale(scale)
		, m_rotation(rotation)
	{}
};

