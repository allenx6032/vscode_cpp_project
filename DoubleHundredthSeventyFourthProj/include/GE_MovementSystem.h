#pragma once

#include "GE_Game.h"

#include "GE_CollisionEvent.h"

#include "GE_TransformComponent.h"
#include "GE_RigidbodyComponent.h"
#include "GE_SpriteComponent.h"
#include "GE_BoxColliderComponent.h"

constexpr float minimumVelocityMagnitudeToKeepUpdating = 2.f;

class MovementSystem : public System
{
public:
	MovementSystem()
	{
		RequireComponent<TransformComponent>();
		RequireComponent<RigidbodyComponent>();
	}

	void SubscribeToEvents(const std::unique_ptr<EventBus>& eventBus)
	{
		eventBus->SubscribeToEvent<CollisionEvent>(this, &MovementSystem::OnCollision);
	}

	void OnCollision(CollisionEvent& eventParams)
	{
		Entity& a = eventParams.m_a;
		Entity& b = eventParams.m_b;

		// enemy and obstacle collision
		if (a.BelongsToGroup("enemies") && b.BelongsToGroup("obstacles"))
		{
			OnEnemyCollidesObstacle(a, b);
		}
		if (b.BelongsToGroup("enemies") && a.BelongsToGroup("obstacles"))
		{
			OnEnemyCollidesObstacle(b, a);
		}
	}

	void OnEnemyCollidesObstacle(Entity& enemy, Entity& obstacle)
	{
		if (enemy.HasComponent<RigidbodyComponent>() && enemy.HasComponent<SpriteComponent>())
		{
			auto& rigidbody = enemy.GetComponent<RigidbodyComponent>();
			auto& sprite = enemy.GetComponent<SpriteComponent>();

			if (rigidbody.m_velocity.x != 0)
			{
				rigidbody.m_velocity.x *= -1;
				sprite.m_flip = (sprite.m_flip == SDL_FLIP_NONE) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
			}
			if (rigidbody.m_velocity.y != 0)
			{
				rigidbody.m_velocity.y *= -1; 
				sprite.m_flip = (sprite.m_flip == SDL_FLIP_NONE) ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
			}
		}
	}

	void Update(double deltaTime)
	{
		for (auto entity : GetSystemEntities())
		{
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rigidbody = entity.GetComponent<RigidbodyComponent>();

			if (!entity.HasTag("player"))
			{
				const bool shouldUpdateVelocity = !rigidbody.ShouldStopMoving();
				const bool frameLongerThanRemainingTimeToUpdate = deltaTime >= rigidbody.GetTimeToStopInSecs();
				if (shouldUpdateVelocity && !frameLongerThanRemainingTimeToUpdate)
				{
					rigidbody.m_velocity -= rigidbody.GetVelocityDecrement() * static_cast<float>(deltaTime);
					{
						const bool shouldStop = glm::length(rigidbody.m_velocity) < minimumVelocityMagnitudeToKeepUpdating;
						if (shouldStop)
						{
							StopEntity(entity);
						}
					}
				}
				else
				{
					StopEntity(entity);
				}
			}

			transform.m_position.x += (rigidbody.m_velocity.x * deltaTime);
			transform.m_position.y += (rigidbody.m_velocity.y * deltaTime);

			const bool isPlayer = entity.HasTag("player");
			if (isPlayer)
			{
				int paddingLeft = 10;
				int paddingTop = 10;
				int paddingRight = 50; // 'pivot' of an entity is top left corner
				int paddingBottom = 50;

				transform.m_position.x = transform.m_position.x < paddingLeft ? paddingLeft : transform.m_position.x;
				transform.m_position.x = transform.m_position.x > Game::m_mapWidth - paddingRight ? Game::m_mapWidth - paddingRight : transform.m_position.x;
				transform.m_position.y = transform.m_position.y < paddingTop ? paddingTop : transform.m_position.y;
				transform.m_position.y = transform.m_position.y > Game::m_mapHeight - paddingBottom ? Game::m_mapHeight - paddingBottom : transform.m_position.y;
			}
			// 'ensure' an entity is only destroyed when it is no longer visible
			const int margin = 80;

			const bool isOutOfMap = 
				transform.m_position.x > Game::m_mapWidth + margin ||
				transform.m_position.x  < 0 - margin ||
				transform.m_position.y > Game::m_mapHeight + margin ||
				transform.m_position.y < 0 - margin;

			if (isOutOfMap && !isPlayer)
			{
				entity.Destroy();
			}
		}
	}

private:
	void StopEntity(Entity& entityToStop)
	{
		entityToStop.GetComponent<RigidbodyComponent>().m_velocity = { 0,0 };
		if (entityToStop.BelongsToGroup("projectiles"))
		{
			entityToStop.GetComponent<BoxColliderComponent>().m_isActive = false;
		}
	}
};