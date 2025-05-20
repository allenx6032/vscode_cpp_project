#pragma once

#include "SDL.h"

#include "GE_ECS.h"

#include "GE_TransformComponent.h"
#include "GE_BoxColliderComponent.h"

class RenderColliderSystem : public System
{
public:
	RenderColliderSystem()
	{
		RequireComponent<TransformComponent>();
		RequireComponent<BoxColliderComponent>();
	}

	void Update(SDL_Renderer* renderer, const SDL_Rect& camera)
	{
		for (auto& entity : GetSystemEntities())
		{
			const auto& transform = entity.GetComponent<TransformComponent>();
			const auto& collider = entity.GetComponent<BoxColliderComponent>();
			SDL_Rect collisionBox{
				static_cast<int>(transform.m_position.x + collider.m_offset.x - camera.x), 
				static_cast<int>(transform.m_position.y + collider.m_offset.y - camera.y), 
				static_cast<int>(collider.m_width),
				static_cast<int>(collider.m_height)};
			
			if (collider.m_isActive)
			{
				if (collider.m_isColliding)
				{
					SDL_SetRenderDrawColor(renderer, 0, 255, 0, 1);
				}
				else
				{
					SDL_SetRenderDrawColor(renderer, 0, 0, 255, 1);
				}
			}
			else
			{
				SDL_SetRenderDrawColor(renderer, 255, 0, 0, 1);
			}
			SDL_RenderDrawRect(renderer, &collisionBox);
		}
	}
};