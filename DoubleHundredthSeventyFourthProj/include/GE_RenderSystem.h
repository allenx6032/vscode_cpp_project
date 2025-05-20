#pragma once

#include <algorithm>

#include "SDL.h"

#include "GE_ECS.h"
#include "GE_AssetStore.h"

#include "GE_TransformComponent.h"
#include "GE_SpriteComponent.h"

class RenderSystem : public System
{
public:
	RenderSystem()
	{
		RequireComponent<TransformComponent>();
		RequireComponent<SpriteComponent>();
	}

	void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore, const SDL_Rect& camera)
	{
		// sort all the entities by their z-index
		struct RenderableEntity
		{
			TransformComponent transformComponent;
			SpriteComponent spriteComponent;
		};

		std::vector<RenderableEntity> renderableEntities;
		for (const auto& entity : GetSystemEntities())
		{
			RenderableEntity renderableEntity;
			renderableEntity.spriteComponent = entity.GetComponent<SpriteComponent>();
			renderableEntity.transformComponent = entity.GetComponent<TransformComponent>();
			
			// we only want to render entities that the camera sees
			const auto& transform = renderableEntity.transformComponent;
			const auto& sprite = renderableEntity.spriteComponent;
			const bool isEntityOutsideCameraView =
				transform.m_position.x + transform.m_scale.x * sprite.m_width < camera.x || 
				transform.m_position.x > camera.x + camera.w || 
				transform.m_position.y + transform.m_scale.y * sprite.m_height < camera.y ||
				transform.m_position.y > camera.y + camera.h;

			if (isEntityOutsideCameraView && !sprite.m_isInCameraSpace) continue;
			
			renderableEntities.emplace_back(renderableEntity);
		}

		std::sort(renderableEntities.begin(), renderableEntities.end(),
			[](const RenderableEntity& first, const RenderableEntity& second)
			{
				return first.spriteComponent.m_zIndex < second.spriteComponent.m_zIndex;
			}
		);

		for (auto entity : renderableEntities)
		{
			const auto transform = entity.transformComponent;
			const auto sprite = entity.spriteComponent;

			SDL_Rect srcRect = sprite.m_textureRect;

			SDL_Rect dstRect = 
			{
				static_cast<int>(transform.m_position.x - (sprite.m_isInCameraSpace ? 0 : camera.x)),
				static_cast<int>(transform.m_position.y - (sprite.m_isInCameraSpace ? 0 : camera.y)),
				static_cast<int>(sprite.m_width * transform.m_scale.x),
				static_cast<int>(sprite.m_height * transform.m_scale.y)
			};
			
			SDL_RenderCopyEx(
				renderer,
				assetStore->GetTexture(sprite.m_assetId),
				&srcRect,
				&dstRect,
				transform.m_rotation,
				NULL,
				sprite.m_flip
			);
		}
	}
};