#pragma once

#include <SDL.h>

#include "GE_ECS.h"

#include "GE_CameraFollowComponent.h"
#include "GE_TransformComponent.h"

// This System will make the camera follow an entity that has it
// there is no protection in case CameraFollowComponent gets added to multiple entities
class CameraMovementSystem : public System
{
public:
	CameraMovementSystem()
	{
		RequireComponent<CameraFollowComponent>();
		RequireComponent<TransformComponent>();
	}

	// the camera is following an entity as long as the map boarders are not crossed
	void Update(SDL_Rect& camera)
	{
		for (auto& entity : GetSystemEntities())
		{
			const auto& entityToFollowPosition = entity.GetComponent<TransformComponent>().m_position;
			
			const bool entityToFollowIsInsideMapX =
				entityToFollowPosition.x  + (camera.w / 2) < Game::m_mapWidth &&
				entityToFollowPosition.x >= 0;

			const bool entityToFollowIsInsideMapY =
				entityToFollowPosition.y + (camera.h / 2) < Game::m_mapHeight &&
				entityToFollowPosition.y >= 0;

			if (entityToFollowIsInsideMapX)
			{
				camera.x = entityToFollowPosition.x - (Game::m_windowWidth / 2);
			}

			if (entityToFollowIsInsideMapY)
			{
				camera.y = entityToFollowPosition.y - (Game::m_windowHeight / 2);
			}

			camera.x = camera.x < 0 ? 0 : camera.x;
			camera.x = camera.x > camera.w ? camera.w : camera.x;
			camera.y = camera.y < 0 ? 0 : camera.y;
			camera.y = camera.y > camera.h ? camera.h : camera.y;
		}
	}
};