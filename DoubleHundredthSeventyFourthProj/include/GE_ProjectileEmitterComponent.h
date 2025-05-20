#pragma once

#include <glm/glm.hpp>
#include <SDL.h>

/*
* \param glm::vec2 velocity = glm::vec2{ 10,10 }
* \param  int frequencyInMs = 1000
* \param int damagePercentage = 10
* \param bool shouldCollideWithPlayer = false
* \param int minVelocityMagnitude = 100
  \param int maxVelocityMagnitude = 400
  \param float timeToReachMaxVelocityInSecs = 0.01f
* */
struct ProjectileEmitterComponent
{
	glm::vec2 m_velocity;
	int m_frequencyInMs;
	int m_damagePercentage; // percentage of life the projectile should take away 
	bool m_shouldCollideWithPlayer;
	int m_lastEmissionTimeInMs;

	ProjectileEmitterComponent(glm::vec2 velocity = glm::vec2{ 10, 10 }, int frequencyInMs = 1000, int damagePercentage = 10, bool shouldCollideWithPlayer = false, int minVelocityMagnitude = 100, int maxVelocityMagnitude = 400, float timeToReachMaxVelocityInSecs = 0.01f)
		: m_velocity(velocity)
		, m_frequencyInMs(frequencyInMs)
		, m_shouldCollideWithPlayer(shouldCollideWithPlayer)
		, m_lastEmissionTimeInMs(SDL_GetTicks())
	{
		// make sure damage is in a valid range
		if (damagePercentage >= 1 && damagePercentage <= 100)
		{
			m_damagePercentage = damagePercentage;
		}
		else
		{
			const int defaultDamagePercentage = 10;
			Logger::InitInfo("ProjectileEmitterComponent: m_damagePercentage not in the range [1, 100] - damage set to " + std::to_string(defaultDamagePercentage));
			m_damagePercentage = defaultDamagePercentage;
		}

		// make sure minimum velocity magnitude is smaller than maximum
		if (minVelocityMagnitude < maxVelocityMagnitude)
		{
			m_MinAndMaxInitialVelocityMagnitude.push_back(minVelocityMagnitude);
			m_MinAndMaxInitialVelocityMagnitude.push_back(maxVelocityMagnitude);
		}
		else
		{
			m_MinAndMaxInitialVelocityMagnitude.push_back(minVelocityMagnitude);
			m_MinAndMaxInitialVelocityMagnitude.push_back(minVelocityMagnitude);
			Logger::InitInfo("Miniminum velocity magnitude is bigger or equal to max velocity magnitude.\n The max velocity was set to be the same as the minimum velocity -> " + std::to_string(minVelocityMagnitude));
		}

		// make sure time to reach max velocity is a positive number
		SetTimeToReachMaxVelocity(timeToReachMaxVelocityInSecs);
	}
	
	inline float GetMinProjectileVelocity() const { return m_MinAndMaxInitialVelocityMagnitude.at(0); }
	inline float GetMaxProjectileVelocity() const { return m_MinAndMaxInitialVelocityMagnitude.at(1); }
	inline float GetTimeToReachMaxVelocityInSecs() const { return m_timeToReachMaxVelocityInSecs; }
	bool HasMaximumVelocityBeenReached() const 
	{ 
		return  SDL_GetTicks() * 0.001f - m_timeWhenPressedInSecs > m_timeToReachMaxVelocityInSecs; 
	}

	void SetTimeToReachMaxVelocity(float timeToReachMaxVelocityInSecs)
	{
		const float maxTimeToReachMaxVelocityInSecs = 4.f;
		const bool timeToReachMaxVelocityInSecsIsValid =
			timeToReachMaxVelocityInSecs < maxTimeToReachMaxVelocityInSecs&&
			timeToReachMaxVelocityInSecs > 0.f
			;
		if (timeToReachMaxVelocityInSecsIsValid)
		{
			m_timeToReachMaxVelocityInSecs = timeToReachMaxVelocityInSecs;
		}
		else
		{
			const float defaultTimeToReachMaxVelocityInSecs = 0.f;
			m_timeToReachMaxVelocityInSecs = defaultTimeToReachMaxVelocityInSecs;
			Logger::InitInfo(" Invalid value for [ProjectileEmitterComponent timeToReachMaxVelocityInSecs] -> "
				+ std::to_string(timeToReachMaxVelocityInSecs) +
				" needs to be in interval [ 0, " + std::to_string(maxTimeToReachMaxVelocityInSecs) +
				" ]. Value was set to " + std::to_string(defaultTimeToReachMaxVelocityInSecs));
		}
	}

	void SetTicks(double ticksWhenPressedDown) 
	{ 
		m_timeWhenPressedInSecs = ticksWhenPressedDown * 0.001f; 
	}

private:
	float m_timeToReachMaxVelocityInSecs;
	float m_timeWhenPressedInSecs;
	std::vector<float> m_MinAndMaxInitialVelocityMagnitude;
};