#pragma once

#include "SDL.h"

#include "GE_Event.h"

class KeyPressedEvent : public Event
{
public:
	SDL_Keycode m_keyPressed;

	KeyPressedEvent(SDL_Keycode keyPressed)
		: m_keyPressed(keyPressed)
	{
	}
};