#pragma once

#include <SDL.h>

#include "GE_ECS.h"
#include "GE_AssetStore.h"

#include "GE_TransformComponent.h"
#include "GE_HealthComponent.h"
#include "GE_SpriteComponent.h"

class RenderHealthBarSystem : public System
{
public:
	RenderHealthBarSystem()
	{
		RequireComponent<TransformComponent>();
		RequireComponent<HealthComponent>();
		RequireComponent<SpriteComponent>();
	}

	void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore, const SDL_Rect& camera)
	{
		for (auto& entity : GetSystemEntities())
		{
			const auto& transform = entity.GetComponent<TransformComponent>();
			const auto& health = entity.GetComponent<HealthComponent>();
			const auto& sprite = entity.GetComponent<SpriteComponent>();

			// health bar
			SDL_Color healthBarColor;
			const auto currentHealth = health.m_currentHealthPertcentage;

			if (currentHealth > 50)		healthBarColor = { 200, 200, 200, 255 };
			else if(currentHealth > 20) healthBarColor = { 255, 165, 0, 255 };
			else										healthBarColor = { 180, 0, 0, 255 };

			SDL_SetRenderDrawColor(renderer, healthBarColor.r, healthBarColor.g, healthBarColor.b, healthBarColor.a);
			

			const Uint16 healthBarWidth = 30;
			const Uint16 healthBarHeight = 3;
			const auto healthBarYOffset = 10;

			const double healthBarPosX = transform.m_position.x - camera.x + (sprite.m_width * transform.m_scale.x) -  healthBarWidth;
			const double healthBarPosY = transform.m_position.y - camera.y - healthBarYOffset;


			SDL_Rect healthBar{
				static_cast<int>(healthBarPosX),
				static_cast<int>(healthBarPosY),
				static_cast<int>(healthBarWidth * health.m_currentHealthPertcentage * .01),
				healthBarHeight
			};

			SDL_RenderFillRect(renderer, &healthBar);

			// health text
			const auto healthAsString = std::to_string(health.m_currentHealthPertcentage) + "%";
			SDL_Surface* surface = TTF_RenderText_Blended(
				assetStore->GetFont(CONST::FONT::pico_8),
				healthAsString.c_str(),
				healthBarColor
			);

			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
			SDL_FreeSurface(surface);

			int labelWidth = 0;
			int labelHeight = 0;
			SDL_QueryTexture(texture, NULL, NULL, &labelWidth, &labelHeight);

			const Uint16 healthTextYOffset = 15;

			SDL_Rect dstRect;
				dstRect = {
					static_cast<int>(healthBarPosX),
					static_cast<int>(healthBarPosY - healthTextYOffset),
					labelWidth,
					labelHeight
				};

			SDL_RenderCopy(renderer, texture, NULL, &dstRect);
			SDL_DestroyTexture(texture);
		}
	}
};