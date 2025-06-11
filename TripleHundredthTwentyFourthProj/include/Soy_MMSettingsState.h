#pragma once
#include "Soy_State.h"
#include "Soy_Gui.h" 
#include "Soy_Background.h"

class MMSettingsState :
	public State
{
public:
	MMSettingsState(StateStuff* stateStuff, sf::Music* mmMusic);
	virtual ~MMSettingsState();

	void update();
	void render();

	void applyChanges(bool ret);

	void pauseState();
	void resumeState();

	
private:
	Gui gui;
	Background* background;
	int selectedObject;

	sf::Music* mmMusic;
};

