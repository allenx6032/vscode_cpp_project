#pragma once
#include "Soy_State.h"
#include "Soy_Gui.h"
#include "Soy_Background.h"

class MainMenuState : public State
{
public:
	MainMenuState(StateStuff* stateStuff);
	virtual ~MainMenuState();

	void update();
	void render();

	void pauseState();
	void resumeState();
private:
	Gui gui;
	int selectedObject;

	sf::Music* mmMusic;

	Background* background;
};

