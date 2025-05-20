#pragma once
#include <memory>
#include <glm/glm.hpp>
#include <sol.hpp>

#include "GE_ECS.h"
#include "GE_AssetStore.h"
#include "GE_EventBus.h"

namespace CONST
{
	namespace FRAMERATE
	{
		constexpr bool IS_CAPPED = true;
		constexpr int FPS = 120;
		constexpr int MILLISECS_PER_FRAME = 1000 / CONST::FRAMERATE::FPS;
	}
	namespace FONT
	{
		constexpr auto charriot_20 = "charriot-font";
		constexpr auto pico_8 = "pico8-font-8";
		constexpr auto pico_10 = "pico8-font-10";
	}
}
//
struct SDL_Window;
struct SDL_Renderer;
struct SDL_Rect;
struct SDL_Color;

class Game
{
public:
	Game();
	~Game() = default;

	void Initialize();
	void Run();
	void Destroy();

	static int m_windowWidth;
	static int m_windowHeight;
	static int m_mapWidth;
	static int m_mapHeight;
private:
	void ProcessInput();
	void Update();
	void Render();

	void Setup();

	SDL_Window* m_window = nullptr;
	SDL_Renderer* m_renderer = nullptr;
	
	sol::state m_lua;
	
	std::unique_ptr<SDL_Rect> m_camera;

	std::unique_ptr<Registry> m_registry;
	std::unique_ptr<AssetStore> m_assetStore;
	std::unique_ptr<EventBus> m_eventBus;

	bool m_IsRunning = false; 
	int m_millisecondsPreviousFrame = 0;

	bool m_shouldRenderDebug = false;

	std::unique_ptr<SDL_Color> m_backgroundColor;
};