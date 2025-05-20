#pragma once

#include <SDL.h>

#include "GE_ECS.h"

#include "GE_AssetStore.h"

#include "GE_TextLabelComponent.h"

class RenderTextSystem : public System
{
public:
	RenderTextSystem()
	{
		RequireComponent<TextLabelComponent>();
	}

	void Update(std::unique_ptr<AssetStore>& assetStore, SDL_Renderer* renderer, const SDL_Rect& camera)
	{
		for (auto& entity : GetSystemEntities())
		{
			const auto& textLabel = entity.GetComponent<TextLabelComponent>();

			const auto font = assetStore->GetFont(textLabel.m_assetName);
			SDL_Surface* surface = TTF_RenderText_Blended(
				font, 
				textLabel.m_text.c_str(),
				textLabel.m_textColor
			);
		
			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
			SDL_FreeSurface(surface);
			
			int labelWidth = 0;
			int labelHeight = 0;
			SDL_QueryTexture(texture, NULL, NULL, &labelWidth, &labelHeight);

			SDL_Rect dstRect = {
				static_cast<int>(textLabel.m_position.x - (textLabel.m_isInCameraSpace ? 0 : camera.x)),
				static_cast<int>(textLabel.m_position.y - (textLabel.m_isInCameraSpace ? 0 : camera.y)),
				labelWidth,
				labelHeight
			};

			SDL_RenderCopy(renderer, texture, NULL, &dstRect);
			SDL_DestroyTexture(texture);
		}
	}
};