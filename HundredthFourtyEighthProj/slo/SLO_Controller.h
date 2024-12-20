#ifndef SCENE_CONTROLLER_H_
#define SCENE_CONTROLLER_H_

#include "SLO_Misc.h"
#include <vector>
#include <memory>

union SDL_Event;
struct SDL_MouseButtonEvent;
struct SDL_KeyboardEvent;
struct SDL_WindowEvent;
struct SDL_JoyHatEvent;
struct SDL_JoyButtonEvent;
class Joystick;
class Controller
{
public:
	UNCOPYABLE(Controller);
	Controller();
	virtual ~Controller();

	virtual void dispatch(const SDL_Event &);
	virtual void onIdle(uint32_t);
	virtual void setJoystickOption();
	virtual void clearJoystickOption();
	bool isIdling() const { return idling_; }

protected:
	void setIdling(bool idling) { idling_ = idling; }

private:
	virtual void onMouseButtonDown(const SDL_MouseButtonEvent &);
	virtual void onMouseButtonUp(const SDL_MouseButtonEvent &);
	virtual void onKeyDown(const SDL_KeyboardEvent &);
	virtual void onKeyUp(const SDL_KeyboardEvent &);
	virtual void onWindow(const SDL_WindowEvent &);
	virtual void onJoyHat(const SDL_JoyHatEvent &);
	virtual void onJoyButtonDown(const SDL_JoyButtonEvent &);
	virtual void onJoyButtonUp(const SDL_JoyButtonEvent &);

	bool idling_;
};

#endif // SCENE_MENU_MENUCONTROLLER_H_
