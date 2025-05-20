#pragma once

#include <imgui.h>
#include <imgui_sdl.h>
#include <glm/glm.hpp>

#include "GE_ECS.h"
#include "GE_AssetStore.h"

#include "GE_TransformComponent.h"
#include "GE_SpriteComponent.h"
#include "GE_RigidbodyComponent.h"
#include "GE_BoxColliderComponent.h"
#include "GE_ProjectileEmitterComponent.h"
#include "GE_HealthComponent.h"
#include "GE_TextLabelComponent.h"

class RenderGUISystem : public System
{
public:
	// not interested in any specific components so no need for RequireComponent<>()
	RenderGUISystem() = default;

	void Update(const std::unique_ptr<Registry>& registry, const std::unique_ptr<AssetStore>& assetStore, SDL_Rect camera)
	{
		using namespace ImGui;

		NewFrame();
		//ShowDemoWindow();
		const auto mapCoordinatesFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize;
		SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
		SetNextWindowBgAlpha(.7f);
		if (Begin("mouse position", nullptr, mapCoordinatesFlags))
		{
			Text("map coordinates [x= %d, y= %d]", 
				static_cast<int>(GetMousePos().x) + camera.x, 
				static_cast<int>(GetMousePos().y) + camera.y);
		}
		End();
		
		if (Begin("Enemy Spawner"))
		{
			// transform
			static int position[2] { 0, 0 };
			static int scale[2] { 1, 1 };
			static int rotation{ 0 };

			// rigidbody
			static int velocity[2]{ 0, 0 };

			// sprite
			const char* assetIds[] = { "truck-image", "tank-image" };
			static int currentAsset = 0;

			// projectile
			static int projectileAngle{ 0 };
			static int projectileVelocity{ 100 };
			static float projectileFrequencyInS{ 3 };
			static float projectileLifetimeInS{ 3 };
			static int projectileDamagePercentage{ 10 };

			// health
			static int initialHealth{ 100 };

			if (CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
			{
				InputInt2("position", position);
				SliderInt2("scale", scale, 0, 10);
				SliderInt("rotation", &rotation, 0, 360, "%d deg");
			}
			Spacing();

			if (CollapsingHeader("Rigidbody", ImGuiTreeNodeFlags_DefaultOpen))
			{
				InputInt2("velocity", velocity);
			}
			Spacing();

			if (CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen))
			{
				Combo("sprite", &currentAsset, assetIds, IM_ARRAYSIZE(assetIds));
			}
			Spacing();

			if (CollapsingHeader("Projectile", ImGuiTreeNodeFlags_DefaultOpen))
			{
				SliderInt("angle", &projectileAngle, 0, 360, "%d deg");
				SliderInt("velocity", &projectileVelocity, 0, 10000, "%d px/s");
				SliderFloat("frequency", &projectileFrequencyInS, 0.1f, 20, "%.1f s");
				SliderFloat("lifetime", &projectileLifetimeInS, 0.5f, 20, "%.1f s");
				SliderInt("damage per projectile", &projectileDamagePercentage, 0, 100, "%d %%");
			}
			Spacing();

			if (CollapsingHeader("Health", ImGuiTreeNodeFlags_DefaultOpen))
			{
				SliderInt("initial health", &initialHealth, 0, 100, "%d %%");
			}
			Spacing();

			if (Button("spawn enemy"))
			{
				auto enemy = registry->CreateEntity();
				enemy.Group("enemies");
				enemy.AddComponent<TransformComponent>(glm::vec2(position[0], position[1]), glm::vec2(scale[0], scale[1]), rotation);
				enemy.AddComponent<RigidbodyComponent>(glm::vec2(velocity[0], velocity[1]));
				enemy.AddComponent<SpriteComponent>(assetIds[currentAsset], 32, 32, 2);
				enemy.AddComponent<BoxColliderComponent>(32, 32);
				float projectileVelX = projectileVelocity * glm::cos(glm::radians(static_cast<float>(projectileAngle)));
				float projectileVelY = projectileVelocity * glm::sin(glm::radians(static_cast<float>(projectileAngle)));
				enemy.AddComponent<ProjectileEmitterComponent>(glm::vec2(projectileVelX, projectileVelY), projectileFrequencyInS * 1000, projectileDamagePercentage, true);
				enemy.AddComponent<HealthComponent>(initialHealth);

				enemy.AddComponent<TextLabelComponent>(glm::vec2(0, -25),  std::to_string(initialHealth), CONST::FONT::pico_10, SDL_Color{ 200, 200, 200, 255 }, false);
			
				position[0], position[1], rotation, projectileAngle = 0;
				scale[0], scale[1] = 1;
				initialHealth = 100;
			}
		}
		End();

		Render();
		ImGuiSDL::Render(GetDrawData());
	}
};