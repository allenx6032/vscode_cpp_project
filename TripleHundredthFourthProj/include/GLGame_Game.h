#pragma once

#define GLEW_STATIC

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <thread>
#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GLGame_GLDebug.h"

// Imgui is already included in the scene editor files

// Also included in the scene editor files
// #include "GLGame_Scene.h"
// #include "GLGame_Rendering.h"

#include "GLGame_Structures.h"
#include "GLGame_SceneEditor.h"
#include "GLGame_Fps.h"
#include "GLGame_Events.h"
#include "GLGame_AABB.h"
#include "GLGame_Macros.h"
#include "GLGame_Object.h"
#include "GLGame_Sprite.h"
#include "GLGame_Background.h"
#include "GLGame_Animation.h"
#include "GLGame_TextureAtlas.h"
#include "GLGame_Log.h"
#include "GLGame_BatchRendering.h"
#include "GLGame_LightBatcher.h"
#include "GLGame_Lighting.h"
#include "GLGame_Particles.h"

namespace GLGame
{
	enum ImGuiStyle
	{
		ImGuiStyleDark,
		ImGuiStyleLight,
		ImGuiStyleClassic,
	};

	namespace GameInternal
	{
		struct _GlobalGameData
		{
			unordered_map <string, Background*>* gBackgrounds;
			unordered_map <uint32_t, Scene*>* gScenes;
			unordered_map <string, Object*>* gObjects;
			unordered_map <string, Sprite*>* gSprites;
		};
	}

	struct CustomEvent
	{
		int m_Event;
		long long m_Frame;
	};

	class Game
	{
	public:

		Game(const Game&) = delete; // To prevent multiple initializations
		Game() : m_CurrentScene(nullptr), m_DisplayFPS(true), m_GameWindow(nullptr), m_KeyHoldEventBuffer(nullptr) {}
		~Game();

		long long GetCurrentFrame() { return m_FpsCount; }
		void Render(bool should_clear = true);
		void StartSceneEditor();

		void SetVSync(bool vsync, bool log = true);
		void DisplayFpsOnWindowTitleBar(bool display);
		void SetCurrentScene(Scene& scene);
		void SetCurrentScene(Scene* scene);
		bool GameWindowShouldClose();

		void SetBlend(bool blend);
		void SetBlendFunction(GLenum blendfuncS, GLenum blendfuncD);

		bool IsThereCollision(Object& obj_1, Object& obj_2);
		bool IsThereCollision(Object& obj_1, AABB aabb_2);
		bool IsThereCollisionOnLayer(Object& obj_1, Object& obj_2, int layer_1, int layer_2);

		// Event handling
		 
		// It is recommended to handle the key hold/repeat events in the OnEvent callback.
		bool KeyIsBeingPressed(int Key);
		
		Scene& GetCurrentScene();
		Scene* GetCurrentScene() const { return m_CurrentScene; };
		Camera* GetCurrentCamera();
		GLFWwindow* GetGameWindow() const { return m_GameWindow; }

		vector<string>* GetGlobalObjectIDs() { return &m_ObjectItemNames; }
		vector<string>* GetGlobalSpriteIDs() { return &m_SpriteItemNames; }

		// Helper Functions : 

		// Is functions
		bool IsVisible(); 
		bool HasFocus(); 
		bool SetFullScreen();

		// Setters
		void SetWindowPosition(const glm::vec2& pos);
		void SetWindowSize(const glm::vec2& size);
		void SetWindowCursorPos(const glm::vec2& pos);

		void MaximizeWindow();
		void RestoreWindow();
		void ShowWindow();
		void HideWindow();

		// Getters
		int GetWindowHeight();
		int GetWindowWidth();

		GLFWwindow* GetWindowHandle() { return m_GameWindow; }
		pair<int, int> GetWindowCenter();

		// Same as GetCursorPos()
		pair<double, double> GetMouseXY();

		// Same as GetMouseXY()
		pair<double, double> GetCursorPos();

		// Returns an AABB struct which can be used for collision testing 
		AABB GetAABBMouseCursor();

		// Gets the window title
		const string& GetWindowCaption() { return m_WindowTitle; }

		// Gets a sprite from the global array based on an id
		Sprite* GetSpriteFromArr(const string& id);

		// Gets an object from the global array based on an id
		Object* GetObjectFromArr(const string& id);

		// Gets background from the global array based on an id
		Background* GetBackgroundFromArr(const string& id);

		// Internal functions.

		// Internal function. Not meant to use.
		void _RegisterObject(Object* object);
		void _DeregisterObject(Object* object);

		// Internal function. Not meant to use.
		void _RegisterSprite(Sprite* sprite);
		void _DeregisterSprite(Sprite* sprite);

		// Internal function. Not meant to use.
		void _RegisterScene(Scene* scene);
		void _DeregisterScene(Scene* scene);

		void _RegisterBackground(Background* bg);
		void _DeregisterBackground(Background* bg);

		// Internal function. Not meant to use.
		void _QueueEvent(Event e);

		// Internal function. Not meant to use.
		GameInternal::_GlobalGameData _GetGlobalData();

	private:
		void PollEvents();
		void ConvertCoordinates(glm::vec3 coordinates, glm::vec3 range);

		bool m_ImGuiInitialized = false;
		bool m_GameAlreadyDestroyed = false;
		vector<Event> m_EventQueue;
		GLFWwindow* m_SceneEditorWindow;

		unordered_map <string, Background*> m_GlobalBackgrounds;
		unordered_map <uint32_t, Scene*> m_GlobalScenes; // Internal vector
		unordered_map <string, Object*> m_GlobalObjects; // Internal vector
		unordered_map <string, Sprite*> m_GlobalSprites; // Internal vector
		vector<string> m_BackgroundNames;
		vector<string> m_ObjectItemNames;
		vector<string> m_SpriteItemNames;

		long long m_FpsCount = 0;
		bool m_OpenGLInitialized = false;
		Shader m_DefaultObjectShader;
		Shader m_DefaultBackgroundShader;

		bool* m_KeyHoldEventBuffer = nullptr;
		
	protected : 
		
		void Init(int w, int h, bool can_resize = true, string title = "Test Game", bool start_SE = false);

		int m_GameWindowWidth;
		int m_GameWindowHeight;
		string m_WindowTitle;

		bool m_DisplayFPS = true;
		bool m_Vsync = false;
		bool m_CanResize = true;

		ImGuiStyle m_IMStyle;
		ImGuiContext* m_IMContext;
		Scene* m_CurrentScene;
		GLFWwindow* m_GameWindow;
		SpriteBatcher* m_SpriteBatcher;
		SpriteBatcher* m_CustomShaderBatcher;
		LightBatcher* m_LightBatcher;

		GameDebugInfo m_DebugInfo;

		// Virtual Functions for event handling
		virtual void OnInitialize(double ts) {}
		virtual void OnGameStart(double ts) {}
		virtual void OnCustomEvent(long long frame) {} // TODO
		virtual void SceneLoader() {}
		virtual void OnObjectMove(Object* object) {} // TODO
		virtual void OnEvent(Event e) {}
		virtual void OnFrameAdvance(long long frame, double ts) {}
		virtual void OnGameDestroy(double ts) {}
	};

	typedef Object Entity;
}