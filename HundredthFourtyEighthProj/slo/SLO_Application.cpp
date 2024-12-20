#include "SLO_Application.h"
#include "SLO_SDLWindow.h"
#include "SLO_Resources.h"
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_log.h>

int Application::return_code_ = 0;

Application::Application(Uint32 flags)
	: is_application_(::SDL_Init(flags) == 0)
	, is_ttf_(::TTF_Init() == 0)
	, is_image_(::IMG_Init(IMG_INIT_PNG) == IMG_INIT_PNG)
	, is_mixer_(::Mix_Init(MIX_INIT_MP3))
	, currentScene_()
	, stackCommand_()
	, manager_()
{
}

Application::~Application()
{
	SDL_::Window::clearWindowMap();
	if (isMixer()) {
		::Mix_Quit();
	}
	if (isImage()) {
		::IMG_Quit();
	}
	if (isTtf()){
		::TTF_Quit();
	}
	if (isApplication()){
		::SDL_Quit();
	}
}

int Application::run(std::shared_ptr<Scene> &&scene)
{
	if(!isApplication()){
		return -1;
	}

	currentScene_ = std::move(scene);
	currentScene_->setTaskManager(manager_);
	currentScene_->onCreate();
	SDL_Event event;
	bool idle(false);
	while(currentScene_) {
		if(::SDL_PollEvent(&event) != 0){
			switch(event.type) {
			case SDL_QUIT:
				currentScene_.reset();
				break;

			case SDL_JOYDEVICEADDED:
				Resources::instance().addJoyDevice(event.jdevice);
				currentScene_->onAddJoystick(event.jdevice.which);
				break;

			case SDL_JOYDEVICEREMOVED:
				Resources::instance().removeJoyDevice(event.jdevice);
				break;

			default:
				currentScene_->dispatch(event);
				idle = true;
				break;
			}
		}
		else if (idle) {
			idle = currentScene_->onIdle(::SDL_GetTicks());
		}
		else {
			if (::SDL_WaitEvent(0) == 0){
				::SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_WaitEvent Error!! %s", ::SDL_GetError());
				quit(1);
			}
		}
		if (currentScene_ && currentScene_->isChangeScene()) {
			auto newScene = currentScene_->getNextScene();
			if (newScene) {
				if (!currentScene_->isFinished()) {
					stackCommand_.push(currentScene_->onSuspend());
				}
				else {
					currentScene_->onDestroy();
				}
				currentScene_->unsetTaskManager();

				currentScene_ = std::move(newScene);
				currentScene_->onCreate();
				currentScene_->setTaskManager(manager_);
			}
			else if (currentScene_->isFinished()) {
				currentScene_->onDestroy();
				currentScene_->unsetTaskManager();
				if(!stackCommand_.empty()){
					auto command = stackCommand_.top();
					currentScene_ = command->resume();
					currentScene_->setTaskManager(manager_);
					stackCommand_.pop();
				}
				else{
					currentScene_.reset();
				}
			}
		}
	}
	clearResumeStack();
	return return_code_;
}

void Application::clearResumeStack()
{
	while(!stackCommand_.empty()){stackCommand_.pop();}
}

//void Application::waitFrame()
//{
//	static const Uint32 wait((1000<<16)/60);
//	static Uint32 lasttime(0); // last time
//	static Uint32 passage(0);
//
//	const Uint32 t(::SDL_GetTicks()); // now time
//	const Uint32 progress(t - lasttime);
//	passage = (passage & 0xffff) + wait;
//	const Uint32 twait(passage >> 16);
//	if(progress >= twait){
//		lasttime = t;
//	}
//	else{
//		::SDL_Delay(twait-progress);
//		while((::SDL_GetTicks()-lasttime) < twait);
//		lasttime += twait;
//	}
//}
