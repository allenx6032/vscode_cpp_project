#pragma once

#include <SDL.h>

#include "GE_ECS.h"

#include "GE_ProjectileComponent.h"
#include "GE_ProjectileEmitterComponent.h"
#include "GE_TransformComponent.h"
#include "GE_RigidbodyComponent.h"
#include "GE_SpriteComponent.h"
#include "GE_BoxColliderComponent.h"

#include "GE_LeftMouseButtonDownEvent.h"
#include "GE_LeftMouseButtonUpEvent.h"

class ProjectileEmitSystem : public System
{
public:
	ProjectileEmitSystem()
	{
		RequireComponent<ProjectileEmitterComponent>();
		RequireComponent<TransformComponent>();
	}

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus)
	{
		eventBus->SubscribeToEvent<LeftMouseButtonDownEvent>(this, &ProjectileEmitSystem::StartCountingTimeFireProjectileButtonHeldDown);
		eventBus->SubscribeToEvent<LeftMouseButtonUpEvent>(this, &ProjectileEmitSystem::FireProjectile);
	}

	void Update(std::unique_ptr<Registry>& registry)
	{
		for (auto& entity : GetSystemEntities())
		{
			auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();

			const bool shouldEmitProjectile = SDL_GetTicks() - projectileEmitter.m_lastEmissionTimeInMs >=  projectileEmitter.m_frequencyInMs;
			const bool isPlayer = entity.HasTag("player");

			if (shouldEmitProjectile && !isPlayer)
			{
				const auto& transform = entity.GetComponent<TransformComponent>();
				
				auto projectile = registry->CreateEntity();
				projectile.Group("projectiles");
				glm::vec2 projectileSpawnPosition = transform.m_position;

				if (entity.HasComponent<SpriteComponent>())
				{
					const auto& sprite = entity.GetComponent<SpriteComponent>();
					projectileSpawnPosition.x += (transform.m_scale.x * sprite.m_width * .5f);
					projectileSpawnPosition.y += (transform.m_scale.y * sprite.m_height * .5f);
				}

				projectile.AddComponent<TransformComponent>(projectileSpawnPosition);
				projectile.AddComponent<RigidbodyComponent>(projectileEmitter.m_velocity);
				projectile.AddComponent<SpriteComponent>("bullet-image", 64, 64, 4);
				projectile.AddComponent<BoxColliderComponent>();
				projectile.AddComponent<ProjectileComponent>(projectileEmitter.m_shouldCollideWithPlayer, projectileEmitter.m_damagePercentage);

				projectileEmitter.m_lastEmissionTimeInMs = SDL_GetTicks();
			}
		}
	}
private:
	void StartCountingTimeFireProjectileButtonHeldDown(LeftMouseButtonDownEvent& eventArgs)
	{
		for (auto& entity : GetSystemEntities())
		{
			const bool entityIsPlayer = entity.HasTag("player");

			if (entityIsPlayer)
			{
				auto& projectileEmitterComponent = entity.GetComponent<ProjectileEmitterComponent>();
				projectileEmitterComponent.SetTicks(SDL_GetTicks());
			}
		}
	}

	void FireProjectile(LeftMouseButtonUpEvent& eventArgs)
	{
		for (auto& entity : GetSystemEntities())
		{
			const bool entityIsPlayer = entity.HasTag("player");
			
			if (entityIsPlayer)
			{
				auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
				const bool shouldEmitProjectile = SDL_GetTicks() - projectileEmitter.m_lastEmissionTimeInMs >= projectileEmitter.m_frequencyInMs;
				
				if (shouldEmitProjectile)
				{

					auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
					const auto& transform = entity.GetComponent<TransformComponent>();
					
					glm::vec2 projectileSpawnPosition = transform.m_position;
					
					if (entity.HasComponent<SpriteComponent>())
					{
						const auto& sprite = entity.GetComponent<SpriteComponent>();
						projectileSpawnPosition.x += (transform.m_scale.x * sprite.m_width * .5f);
						projectileSpawnPosition.y += (transform.m_scale.y * sprite.m_height * .5f);
					}
					glm::vec2 projectileVelocity;

					auto GetMousePosition = []() -> glm::vec2
					{
						int x = 204, y = 301;
						SDL_GetMouseState(&x, &y);
						return glm::vec2{ x, y };
					};

					// direction 
					const glm::vec2 cameraPosition{ eventArgs.m_cameraPositon.x, eventArgs.m_cameraPositon.y };
					const glm::vec2 mousePositionInScreenCoords{ GetMousePosition() - cameraPosition };
					const glm::vec2 projectileDirection = mousePositionInScreenCoords - projectileSpawnPosition;

					// velocity magnitude 
					const bool maxVelocityReached = projectileEmitter.HasMaximumVelocityBeenReached();
					const float projectileVelocityMagnitude = maxVelocityReached ?
						projectileEmitter.GetMaxProjectileVelocity() :
						projectileEmitter.GetMinProjectileVelocity(); 

					// velocity vector
					projectileVelocity = (glm::length(projectileDirection) != 0) ?
						glm::normalize(mousePositionInScreenCoords - projectileSpawnPosition) * projectileVelocityMagnitude:
						glm::vec2{0,0}
						;

					auto projectile = entity.m_registry->CreateEntity();
					projectile.Group("projectiles");
					
					const float scaleFactorX = 0.15f;
					const float scaleFactorY = 0.15f;
					projectile.AddComponent<TransformComponent>(projectileSpawnPosition, glm::vec2(scaleFactorX, scaleFactorY));
					
					const float timeToStopInSecs = .6f;
					projectile.AddComponent<RigidbodyComponent>(projectileVelocity, timeToStopInSecs);
					projectile.GetComponent<RigidbodyComponent>().WasPushed();
					
					const int spriteWidth = 64;
					const int spriteHeight = 64;
					projectile.AddComponent<SpriteComponent>("bullet-image", spriteWidth, spriteHeight, 4);
					
					const auto colliderWidth = spriteWidth * scaleFactorX;
					const auto colliderHeight = spriteHeight * scaleFactorY;
					projectile.AddComponent<BoxColliderComponent>(colliderWidth, colliderHeight);
					
					projectile.AddComponent<ProjectileComponent>(projectileEmitter.m_shouldCollideWithPlayer, projectileEmitter.m_damagePercentage);
				
					projectileEmitter.m_lastEmissionTimeInMs = SDL_GetTicks();
				}
			}
		}
	}
};