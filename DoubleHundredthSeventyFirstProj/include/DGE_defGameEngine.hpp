#pragma once

#ifndef DEF_GAME_ENGINE_HPP
#define DEF_GAME_ENGINE_HPP

#pragma GCC diagnostic ignored "-Wunknown-pragmas"

#ifdef __EMSCRIPTEN__
#define DGE_PLATFORM_EMSCRIPTEN
#else
#define DGE_PLATFORM_GL
#define DGE_PLATFORM_GLFW3
#endif

#ifdef _WIN32

#define _CRT_SECURE_NO_WARNINGS

#ifndef __MINGW32__
#ifdef DGE_PLATFORM_GL
#pragma comment(lib, "opengl32.lib")
#endif

#ifdef DGE_PLATFORM_GLFW3
#pragma comment(lib, "glfw3.lib")
#endif
#endif

#else
/*
* link libraries
* to your project
*/
#endif

#include "DGE_Pch.hpp"

#ifndef DGE_IGNORE_VECTOR2D
#include "DGE_Vector2D.hpp"
#endif

#include "DGE_Pixel.hpp"
#include "DGE_Sprite.hpp"
#include "DGE_Texture.hpp"
#include "DGE_Graphic.hpp"

#ifdef DGE_PLATFORM_GL
#include "DGE_PlatformGL.hpp"
#endif

#ifdef DGE_PLATFORM_GLFW3
#include "DGE_PlatformGLFW3.hpp"
#endif

#ifdef DGE_PLATFORM_EMSCRIPTEN
#include "DGE_PlatformEmscripten.hpp"
#endif

#include "DGE_Layer.hpp"
#include "DGE_Window.hpp"
#include "DGE_InputHandler.hpp"
#include "DGE_Console.hpp"

namespace def
{
	using TimePoint = std::chrono::system_clock::time_point;

	class Platform;
	class Console;

	class GameEngine
	{
	public:
		GameEngine();
		virtual ~GameEngine();

	#ifdef DGE_PLATFORM_GLFW3
		friend class PlatformGLFW3;
	#endif

	#ifdef DGE_PLATFORM_EMSCRIPTEN
		friend class PlatformEmscripten;
	#endif

		friend class Console;
		friend class InputHandler;

	private:
		bool m_IsAppRunning;
		bool m_OnlyTextures;

		Graphic m_Font;
		int m_TabSize;

		std::vector<Layer> m_Layers;

		size_t m_PickedLayer;
		size_t m_ConsoleLayer;

		Pixel m_BackgroundColour;

		float m_DeltaTime;
		float m_TickTimer;

		std::shared_ptr<Platform> m_Platform;

		TimePoint m_TimeStart;
		TimePoint m_TimeEnd;

		std::shared_ptr<InputHandler> m_Input;
		std::shared_ptr<Window> m_Window;
		std::unique_ptr<Console> m_Console;

	#ifndef PLATFORM_EMSCRIPTEN
		uint32_t m_FramesCount;
	#endif

		static constexpr size_t CIRCLE_VERTICES_COUNT = 64;
		static std::vector<Vector2f> sm_UnitCircle;

	public:
		// Is used internally
		static GameEngine* s_Engine;

		// Is called before the main loop
		virtual bool OnUserCreate() = 0;

		// Is being called on every frame
		virtual bool OnUserUpdate(float deltaTime) = 0;

		// Is being called after clearing the screen buffer and sending
		// all textures to the pipeline and before flushing the screen
		virtual bool OnAfterDraw();

		// Is executed when a user presses an ENTER key
		virtual void OnTextCapturingComplete(const std::string& text);

		// Is executed when a user presses the ENTER key when a console is opened
		virtual bool OnConsoleCommand(const std::string& command, std::stringstream& output, Pixel& colour);

		// Constructs a window
		bool Construct(int screenWidth, int screenHeight, int pixelWidth, int pixelHeight, bool fullScreen = false, bool vsync = false, bool dirtyPixel = true);
		
		// Must be called only after Construct method, basically starts the main loop
		void Run();

	private:
		// Frees memory
		void Destroy();

		// The main loop of a program (handles input, draws to the screen)
		void MainLoop();

		// Constructs a unit circle using trigonometry functions
		static void MakeUnitCircle(std::vector<Vector2f>& circle, size_t verts);

	public:
		// Drawing routines

		bool Draw(const Vector2i& pos, const Pixel& col = WHITE);
		virtual bool Draw(int x, int y, const Pixel& col = WHITE);

		void DrawLine(const Vector2i& pos1, const Vector2i& pos2, const Pixel& col = WHITE);
		virtual void DrawLine(int x1, int y1, int x2, int y2, const Pixel& col = WHITE);

		void DrawTriangle(const Vector2i& pos1, const Vector2i& pos2, const Vector2i& pos3, const Pixel& col = WHITE);
		virtual void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, const Pixel& col = WHITE);

		void FillTriangle(const Vector2i& pos1, const Vector2i& pos2, const Vector2i& pos3, const Pixel& col = WHITE);
		virtual void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, const Pixel& col = WHITE);

		void DrawRectangle(const Vector2i& pos, const Vector2i& size, const Pixel& col = WHITE);
		virtual void DrawRectangle(int x, int y, int sizeX, int sizeY, const Pixel& col = WHITE);

		void FillRectangle(const Vector2i& pos, const Vector2i& size, const Pixel& col = WHITE);
		virtual void FillRectangle(int x, int y, int sizeX, int sizeY, const Pixel& col = WHITE);

		void DrawCircle(const Vector2i& pos, int radius, const Pixel& col = WHITE);
		virtual void DrawCircle(int x, int y, int radius, const Pixel& col = WHITE);

		void FillCircle(const Vector2i& pos, int radius, const Pixel& col = WHITE);
		virtual void FillCircle(int x, int y, int radius, const Pixel& col = WHITE);

		void DrawEllipse(const Vector2i& pos, const Vector2i& size, const Pixel& col = WHITE);
		virtual void DrawEllipse(int x, int y, int sizeX, int sizeY, const Pixel& col = WHITE);

		void FillEllipse(const Vector2i& pos, const Vector2i& size, const Pixel& col = WHITE);
		virtual void FillEllipse(int x, int y, int sizeX, int sizeY, const Pixel& col = WHITE);

		void DrawSprite(const Vector2i& pos, const Sprite* sprite);
		virtual void DrawSprite(int x, int y, const Sprite* sprite);

		void DrawPartialSprite(const Vector2i& pos, const Vector2i& filePos, const Vector2i& fileSize, const Sprite* sprite);
		virtual void DrawPartialSprite(int x, int y, int fileX, int fileY, int fileSizeX, int fileSizeY, const Sprite* sprite);

		void DrawWireFrameModel(const std::vector<Vector2f>& modelCoordinates, const Vector2f& pos, float rotation = 0.0f, float scale = 1.0f, const Pixel& col = WHITE);
		virtual void DrawWireFrameModel(const std::vector<Vector2f>& modelCoordinates, float x, float y, float rotation = 0.0f, float scale = 1.0f, const Pixel& col = WHITE);

		void FillWireFrameModel(const std::vector<Vector2f>& modelCoordinates, const Vector2f& pos, float rotation = 0.0f, float scale = 1.0f, const Pixel& col = WHITE);
		virtual void FillWireFrameModel(const std::vector<Vector2f>& modelCoordinates, float x, float y, float rotation = 0.0f, float scale = 1.0f, const Pixel& col = WHITE);

		void DrawString(const Vector2i& pos, std::string_view text, const Pixel& col = WHITE, const Vector2i& scale = { 1, 1 });
		virtual void DrawString(int x, int y, std::string_view text, const Pixel& col = WHITE, int scaleX = 1, int scaleY = 1);

		virtual void Clear(const Pixel& col);
		void ClearTexture(const Pixel& col);

		void DrawTexture(const Vector2f& pos, const Texture* tex, const Vector2f& scale = { 1.0f, 1.0f }, const Pixel& tint = WHITE);
		void DrawPartialTexture(const Vector2f& pos, const Texture* tex, const Vector2f& filePos, const Vector2f& fileSize, const Vector2f& scale = { 1.0f, 1.0f }, const Pixel& tint = WHITE);

		void DrawWarpedTexture(const std::vector<Vector2f>& points, const Texture* tex, const Pixel& tint = WHITE);

		void DrawRotatedTexture(const Vector2f& pos, const Texture* tex, float rotation, const Vector2f& center = { 0.0f, 0.0f }, const Vector2f& scale = { 1.0f, 1.0f }, const Pixel& tint = WHITE);
		void DrawPartialRotatedTexture(const Vector2f& pos, const Texture* tex, const Vector2f& filePos, const Vector2f& fileSize, float rotation, const Vector2f& center = { 0.0f, 0.0f }, const Vector2f& scale = { 1.0f, 1.0f }, const Pixel& tint = WHITE);

		void DrawTexturePolygon(const std::vector<Vector2f>& verts, const std::vector<Pixel>& cols, Texture::Structure structure);

		void DrawTextureLine(const Vector2i& pos1, const Vector2i& pos2, const Pixel& col = WHITE);

		void DrawTextureTriangle(const Vector2i& pos1, const Vector2i& pos2, const Vector2i& pos3, const Pixel& col = WHITE);
		void DrawTextureRectangle(const Vector2i& pos, const Vector2i& size, const Pixel& col = WHITE);
		void DrawTextureCircle(const Vector2i& pos, int radius, const Pixel& col = WHITE);

		void FillTextureTriangle(const Vector2i& pos1, const Vector2i& pos2, const Vector2i& pos3, const Pixel& col = WHITE);
		void FillTextureRectangle(const Vector2i& pos, const Vector2i& size, const Pixel& col = WHITE);
		void FillTextureCircle(const Vector2i& pos, int radius, const Pixel& col = WHITE);

		void GradientTextureTriangle(const Vector2i& pos1, const Vector2i& pos2, const Vector2i& pos3, const Pixel& col1 = WHITE, const Pixel& col2 = WHITE, const Pixel& col3 = WHITE);
		void GradientTextureRectangle(const Vector2i& pos, const Vector2i& size, const Pixel& colTL = WHITE, const Pixel& colTR = WHITE, const Pixel& colBR = WHITE, const Pixel& colBL = WHITE);

		void DrawTextureString(const Vector2i& pos, std::string_view text, const Pixel& col = WHITE, const Vector2f& scale = { 1.0f, 1.0f });

		// Drawing targets

		void SetDrawTarget(Graphic* target);
		Graphic* GetDrawTarget();

		// Pixel modes

		void SetPixelMode(Pixel::Mode pixelMode);
		Pixel::Mode GetPixelMode() const;

		// Texture stuff

		void SetTextureStructure(Texture::Structure textureStructure);
		Texture::Structure GetTextureStructure() const;
		void UseOnlyTextures(bool enable);

		// Shaders

		void SetShader(Pixel(*func)(const Vector2i&, const Pixel&, const Pixel&));

		// Timings

		float GetDeltaTime() const;

		// Window's stuff

	#if defined(DGE_PLATFORM_GLFW3)
		GLFWwindow* GetNativeWindow();
	#elif defined(DGE_PLATFORM_EMSCRIPTEN)
		EGLDisplay& GetNativeWindow();
	#endif

		// Layers' stuff

		size_t CreateLayer(const Vector2i& offset, const Vector2i& size, bool update = true, bool visible = true, const Pixel& tint = WHITE);
		void PickLayer(size_t layer);
		size_t GetPickedLayer() const;
		Layer* GetLayerByIndex(size_t index);

		// Window, input and console stuff

		Window *const GetWindow();
		InputHandler *const GetInput();
		Console *const GetConsole();

	};
}

#endif
