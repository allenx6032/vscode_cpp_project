#pragma once

#include "GE_ECS.h"

#include "GE_TransformComponent.h"
#include "GE_BoxColliderComponent.h"

#include "GE_EventBus.h"
#include "GE_CollisionEvent.h"

class CollisionSystem : public System
{
public:
	CollisionSystem()
	{
		RequireComponent<TransformComponent>();
		RequireComponent<BoxColliderComponent>();
	}

	// not a fan of this arg, violates R.34 of guidelines since eventBus is not being reseated
	void Update(std::unique_ptr<EventBus>& eventBus)
	{
		auto systemEntities = GetSystemEntities();
		for (auto firstEntityIterator = systemEntities.begin(); firstEntityIterator != systemEntities.end(); firstEntityIterator++)
		{
			const Entity first = *firstEntityIterator;
			const auto& firstTransform = first.GetComponent<TransformComponent>();
			auto& firstCollider = first.GetComponent<BoxColliderComponent>();

			if (firstCollider.m_isActive)
			{
				for (auto secondEntityIterator = firstEntityIterator; secondEntityIterator != systemEntities.end(); secondEntityIterator++)
				{
					const Entity second = *secondEntityIterator;
					const bool isNotSameEntity = first != second;
					if (isNotSameEntity)
					{
						const auto& secondTransform = second.GetComponent<TransformComponent>();
						auto& secondCollider = second.GetComponent<BoxColliderComponent>();

						if (secondCollider.m_isActive)
						{
							const bool collisionDetected = CheckAABBCollision(firstTransform, firstCollider, secondTransform, secondCollider);
							if (collisionDetected)
							{
								eventBus->EmitEvent<CollisionEvent>(first, second);
								firstCollider.m_isColliding = true;
								secondCollider.m_isColliding = true;
								//Logger::Log("emitted collision");
							}
							else
							{
								firstCollider.m_isColliding = false;
								secondCollider.m_isColliding = false;
							}
						}
					}
				}
			}
		}
	}

	bool CheckAABBCollision(const TransformComponent& firstTransform, const BoxColliderComponent& firstCollider, const TransformComponent& secondTransform, const BoxColliderComponent& secondCollider)
	{
		bool isColliding = false;
		// check if there is collision on the XX axis					
		const bool xIsOverlapping =
			firstTransform.m_position.x + firstCollider.m_offset.x <= secondTransform.m_position.x + secondCollider.m_offset.x  + secondCollider.m_width
			&& (firstTransform.m_position.x + firstCollider.m_offset.x + firstCollider.m_width ) >= secondTransform.m_position.x + secondCollider.m_offset.x;

		if (!xIsOverlapping)
		{
			return isColliding;
		}

		//check if there is collision on YY axis
		const bool yIsOverlapping = 
			firstTransform.m_position.y + firstCollider.m_offset.y < secondTransform.m_position.y + secondCollider.m_offset.y + secondCollider.m_height
			&& (firstTransform.m_position.y + firstCollider.m_offset.y + firstCollider.m_height) >= secondTransform.m_position.y + secondCollider.m_offset.y;
		
		if (xIsOverlapping && yIsOverlapping)
		{
			isColliding = true;
		}
		return isColliding;
	}
};