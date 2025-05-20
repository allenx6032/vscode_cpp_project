#pragma once

#include "GE_Event.h"

struct LeftMouseButtonUpEvent : public Event
{
public:
	SDL_Rect m_cameraPositon;

	LeftMouseButtonUpEvent(const SDL_Rect& cameraPosition)
		: m_cameraPositon(cameraPosition)
	{
	}
};