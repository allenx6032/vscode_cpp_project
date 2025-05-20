#pragma once

struct HealthComponent
{
	int m_currentHealthPertcentage;

	HealthComponent(int healthPercentage = 100)
		:m_currentHealthPertcentage(healthPercentage)
	{
	}
};