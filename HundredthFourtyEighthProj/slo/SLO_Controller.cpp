#include "SLO_Controller.h"
#include "SLO_SDLJoystick.h"
#include "SLO_Application.h"
#include <SDL_events.h>

Controller::Controller()
	: idling_(false)
{
}

Controller::~Controller()
{
}

void Controller::dispatch(const SDL_Event &event)
{
	switch(event.type){
	case SDL_MOUSEBUTTONDOWN:
		onMouseButtonDown(event.button);
		break;

	case SDL_MOUSEBUTTONUP:
		onMouseButtonUp(event.button);
		break;

	case SDL_KEYDOWN:
		onKeyDown(event.key);
		break;

	case SDL_KEYUP:
		onKeyUp(event.key);
		break;

	case SDL_WINDOWEVENT:
		onWindow(event.window);
		break;

	case SDL_JOYHATMOTION:
		onJoyHat(event.jhat);
		break;

	case SDL_JOYBUTTONDOWN:
		onJoyButtonDown(event.jbutton);
		break;

	case SDL_JOYBUTTONUP:
		onJoyButtonUp(event.jbutton);
		break;

	default:
		break;
	}
}

void Controller::onIdle(uint32_t tick)
{
}

void Controller::onMouseButtonDown(const SDL_MouseButtonEvent &button)
{
}

void Controller::onMouseButtonUp(const SDL_MouseButtonEvent &button)
{
}

void Controller::onKeyDown(const SDL_KeyboardEvent &key)
{
//	switch(key.keysym.sym)
//	{
//	case SDLK_ESCAPE:
//		break;
//
//	case SDLK_BACKSPACE:
//		break;
//
//	case SDLK_RETURN:
//		break;
//
//	case SDLK_UP:
//		break;
//
//	case SDLK_DOWN:
//		break;
//
//	case SDLK_LEFT:
//		break;
//
//	case SDLK_RIGHT:
//		break;
//
//	default:
//		break;
//	}
}

void Controller::onKeyUp(const SDL_KeyboardEvent &key)
{
}

void Controller::onWindow(const SDL_WindowEvent &window)
{
//	switch(window.event)
//	{
//	case SDL_WINDOWEVENT_EXPOSED:
//		break;
//
//	case SDL_WINDOWEVENT_CLOSE:
//		break;
//
//	default:
//		break;
//	}
}

void Controller::onJoyHat(const SDL_JoyHatEvent &jhat)
{
}

void Controller::onJoyButtonDown(const SDL_JoyButtonEvent &jbutton)
{
//	std::cout << "JoyButton button = " << static_cast<int>(jbutton.button) << ", state = " << static_cast<int>(jbutton.state) << std::endl;
}

void Controller::onJoyButtonUp(const SDL_JoyButtonEvent &jbutton)
{
}

void Controller::setJoystickOption()
{
}

void Controller::clearJoystickOption()
{
}
