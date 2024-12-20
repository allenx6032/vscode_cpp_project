#include "SLO_OpeningController.h"
#include "SLO_OpeningDataStore.h"
#include "SLO_OpeningUseCase.h"
#include "SLO_SDLJoystick.h"
#include <SDL_events.h>

OpeningController::OpeningController(OpeningUseCase &model, const OpeningDataStore &dataStore)
	: Controller()
	, useCase_(model)
	, dataStore_(dataStore)
{
}

OpeningController::~OpeningController()
{
}

void OpeningController::onIdle(uint32_t tick)
{
	useCase_.action(tick);
}

void OpeningController::onKeyDown(const SDL_KeyboardEvent &key)
{
	switch(key.keysym.sym)
	{
	case SDLK_ESCAPE:
		useCase_.quit();
		break;

	case SDLK_BACKSPACE:
		useCase_.pushCancel();
		break;

	case SDLK_RETURN:
		useCase_.pushEnter();
		break;

	case SDLK_UP:
		useCase_.pushUp();
		break;

	case SDLK_DOWN:
		useCase_.pushDown();
		break;

	case SDLK_LEFT:
		useCase_.pushLeft();
		break;

	case SDLK_RIGHT:
		useCase_.pushRight();
		break;

	default:
		break;
	}
}

void OpeningController::onWindow(const SDL_WindowEvent &window)
{
	switch(window.event)
	{
	case SDL_WINDOWEVENT_EXPOSED:
		useCase_.exposed();
		break;

	case SDL_WINDOWEVENT_CLOSE:
		useCase_.finish();
		break;

	default:
		break;
	}
}

void OpeningController::onJoyButtonDown(const SDL_JoyButtonEvent &jbutton)
{
	auto joystick = dataStore_.getJoystick(0);
	if (joystick && joystick->isJoystick()) {
		if (jbutton.button == dataStore_.getEnterButton()) {
			useCase_.pushEnter();
		}
		else if (jbutton.button == dataStore_.getCancelButton()) {
			useCase_.finish();
		}
	}
}
