#pragma once

#include "GE_ECS.h"

#include "GE_ProjectileComponent.h"
#include "GE_RigidbodyComponent.h"
#include "GE_BoxColliderComponent.h"

class ProjectileLifeCycleSystem : public System
{
public:
	ProjectileLifeCycleSystem()
	{
		RequireComponent<ProjectileComponent>();
		RequireComponent<RigidbodyComponent>();
		RequireComponent<BoxColliderComponent>();
	}

	void Update(double deltaTime)
	{
		for (auto& entity : GetSystemEntities())
		{
			if (entity.GetComponent<BoxColliderComponent>().m_isActive)
			{
				if (entity.GetComponent<RigidbodyComponent>().IsStopped())
				{
					entity.GetComponent<BoxColliderComponent>().m_isActive = false;
				}
			}
		}
	}
};