#pragma once

#include "glm/glm.hpp"

// all these vec2 are to be used in case we want to give a different velocity to each direction
struct KeyboardControlledComponent
{
	glm::vec2 m_upVelocity;
	glm::vec2 m_rightVelocity;
	glm::vec2 m_downVelocity;
	glm::vec2 m_leftVelocity;

	KeyboardControlledComponent(glm::vec2 upVelocity = glm::vec2(0), glm::vec2 rightVelocity = glm::vec2(0), glm::vec2 downVelocity = glm::vec2(0), glm::vec2 leftVelocity = glm::vec2(0))
		: m_upVelocity(upVelocity)
		, m_rightVelocity(rightVelocity)
		, m_downVelocity(downVelocity)
		, m_leftVelocity(leftVelocity)
	{
	}
};