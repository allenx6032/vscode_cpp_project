#ifndef SCENE_MENU_MENUCONTROLLER_H_
#define SCENE_MENU_MENUCONTROLLER_H_

#include "SLO_Controller.h"
#include <vector>
#include <memory>

class MenuUseCase;
class MenuDataStore;
class MenuController : public Controller
{
public:
	MenuController(MenuUseCase &, const MenuDataStore &dataStore);
	virtual ~MenuController() override;

	virtual void onIdle(uint32_t) override;
	virtual void setJoystickOption() override;
	virtual void clearJoystickOption() override;

private:
	virtual void onKeyDown(const SDL_KeyboardEvent &) override;
	virtual void onWindow(const SDL_WindowEvent &) override;
	virtual void onJoyHat(const SDL_JoyHatEvent &) override;
	virtual void onJoyButtonDown(const SDL_JoyButtonEvent &) override;

	MenuUseCase &useCase_;
	const MenuDataStore &dataStore_;
};

#endif // SCENE_MENU_MENUCONTROLLER_H_
