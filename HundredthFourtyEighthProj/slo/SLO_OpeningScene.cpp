#include "SLO_OpeningScene.h"
#include "SLO_OpeningController.h"
#include "SLO_OpeningPresenter.h"
#include "SLO_OpeningUseCase.h"
#include "SLO_SDLWindow.h"
#include <SDL_events.h>
#include <SDL_opengl.h>

OpeningScene::OpeningScene(uint32_t windowId)
	: Scene(windowId)
	, dataStore_()
	, presenter_(*this, dataStore_)
	, useCase_(presenter_, dataStore_)
	, controller_(useCase_, dataStore_)
{
}

void OpeningScene::dispatch(const SDL_Event &event)
{
//	if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_F5) {
//		reload();
//	}
	controller_.dispatch(event);
}

bool OpeningScene::onIdle(uint32_t tick)
{
	controller_.onIdle(tick);
	bool result = Scene::onIdle(tick);
	return result || controller_.isIdling() || presenter_.isIdling();
}

void OpeningScene::onCreate()
{
	Scene::onCreate();
	controller_.setJoystickOption();
	useCase_.onCreate();
}

void OpeningScene::onDestroy()
{
	controller_.clearJoystickOption();
	Scene::onDestroy();
}

void OpeningScene::onAddJoystick(int index)
{
	controller_.setJoystickOption();
}

void OpeningScene::onResume(int pos)
{
	controller_.setJoystickOption();
	useCase_.onResume(pos);
}

std::shared_ptr<SceneResumeCommand> OpeningScene::onSuspend()
{
	return std::make_shared<OpeningSceneResumeCommand>(getWindow()->getWindowId(), useCase_.getPos());
}

std::shared_ptr<Scene> OpeningSceneResumeCommand::resume() {
	auto result = std::make_shared<OpeningScene>(windowId_);
	result->onResume(pos_);
	return result;
}

OpeningSceneResumeCommand::OpeningSceneResumeCommand(uint32_t windowId, int pos)
	: windowId_(windowId)
	, pos_(pos)
{
}
