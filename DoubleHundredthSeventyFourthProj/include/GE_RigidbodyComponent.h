#pragma once

#include <glm/glm.hpp>
#include <SDL.h>

/*
* \param glm::vec2 velocity = (0,0)
* \param float timeToStopInSecs = .5f - seconds it takes for the entity to come to a full stop after being pushed
*/
struct RigidbodyComponent
{
	glm::vec2 m_velocity;
	glm::vec2 m_initialVelocity;

	RigidbodyComponent(glm::vec2 velocity = glm::vec2(0, 0), float timeToStopInSecs = .5f)
		: m_velocity(velocity)
		, m_initialVelocity(velocity)
		, m_timeToStopInSecs(timeToStopInSecs)
		, m_timeOfLastPushInSecs(0)
	{
		SetTimeToStopInSecs(timeToStopInSecs);
	}

	inline float GetTimeToStopInSecs() const { return m_timeToStopInSecs; }
	inline void SetTimeToStopInSecs(float timeToStopInSecs) { m_timeToStopInSecs = (timeToStopInSecs > 0) ? timeToStopInSecs : 0.1f; CalculateVelocityDecrement(); }
	
	inline bool IsStopped() const { return m_velocity.x == 0 && m_velocity.y == 0; }
	inline bool ShouldStopMoving() const { return SDL_GetTicks() * .001 - m_timeOfLastPushInSecs > m_timeToStopInSecs; }
	void WasPushed() { m_timeOfLastPushInSecs = SDL_GetTicks() * 0.001; CalculateVelocityDecrement(); }

	const glm::vec2& GetVelocityDecrement() const { return m_velocityDecrement; }
private:
	inline void CalculateVelocityDecrement() { m_velocityDecrement = (m_velocity / m_timeToStopInSecs); }

	double m_timeOfLastPushInSecs;
	float m_timeToStopInSecs;
	glm::vec2 m_velocityDecrement;
};