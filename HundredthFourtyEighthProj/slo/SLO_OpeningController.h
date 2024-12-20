#ifndef SCENE_OPENING_OPENINGCONTROLLER_H_
#define SCENE_OPENING_OPENINGCONTROLLER_H_

#include "SLO_Controller.h"

class OpeningUseCase;
class OpeningDataStore;
class OpeningController : public Controller
{
public:
	OpeningController(OpeningUseCase &, const OpeningDataStore &dataStore);
	virtual ~OpeningController() override;

	virtual void onIdle(uint32_t) override;

private:
	virtual void onKeyDown(const SDL_KeyboardEvent &) override;
	virtual void onWindow(const SDL_WindowEvent &) override;
	virtual void onJoyButtonDown(const SDL_JoyButtonEvent &) override;

	OpeningUseCase &useCase_;
	const OpeningDataStore &dataStore_;
};

#endif // SCENE_OPENING_OPENINGCONTROLLER_H_
