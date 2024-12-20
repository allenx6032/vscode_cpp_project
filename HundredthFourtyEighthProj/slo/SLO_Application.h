#if !defined(APPLICATION_H_)
#define APPLICATION_H_

#include "SLO_Misc.h"
#include "SLO_Scene.h"
#include "SLO_TaskManager.h"
#include <SDL.h>
#include <memory>
#include <stack>

class Application
{
public:
	UNCOPYABLE(Application);
	explicit Application(Uint32 flags);
	~Application();

	bool initSubSystem(Uint32 flags) { return ::SDL_InitSubSystem(flags) == 0; }
	void quitSubSystem(Uint32 flags) { ::SDL_QuitSubSystem(flags); }
	bool isApplication() const { return is_application_; }
	bool isTtf() const { return is_ttf_; }
	bool isImage() const { return is_image_; }
	bool isMixer() const { return is_mixer_; }
	int run(std::shared_ptr<Scene> &&);
	void clearResumeStack();

	//static void waitFrame();
	static void quit(const int val = 0)
	{
		SDL_Event event = {SDL_QUIT};
		return_code_ = val;
		::SDL_PushEvent(&event);
	}

	static void updateWindow(Uint32 id)
	{
		SDL_Event event = {SDL_WINDOWEVENT};
		event.window.windowID = id;
		event.window.event = SDL_WINDOWEVENT_EXPOSED;
		::SDL_PushEvent(&event);
	}

	static uint32_t getTickCount() { return ::SDL_GetTicks(); }

private:
	const bool is_application_;
	const bool is_ttf_;
	const bool is_image_;
	const bool is_mixer_;
	std::shared_ptr<Scene> currentScene_;
	std::stack<std::shared_ptr<SceneResumeCommand>> stackCommand_;
	TaskManager manager_;
	static int return_code_;
};

#endif // APPLICATION_H_
