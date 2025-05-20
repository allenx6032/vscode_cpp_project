#pragma once

#include <SDL.h>

/*
* \param bool shouldCollideWithPlayer = false
* \param int damagePercentage = 10
* */
struct ProjectileComponent
{
	bool m_shouldCollideWithPlayer;
	int m_damagePercentage;
	int m_projectileStartTimeInMs;

	ProjectileComponent(bool shouldCollideWithPlayer = false, int damagePercentage = 10)
		: m_shouldCollideWithPlayer(shouldCollideWithPlayer)
		, m_damagePercentage(damagePercentage)
		, m_projectileStartTimeInMs(SDL_GetTicks())
	{

	}
};