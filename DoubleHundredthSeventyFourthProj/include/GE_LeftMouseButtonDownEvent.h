#pragma once

#include "GE_Event.h"

struct LeftMouseButtonDownEvent : public Event
{
public:
	SDL_Rect m_cameraPositon;

	LeftMouseButtonDownEvent(const SDL_Rect& cameraPosition)
		: m_cameraPositon(cameraPosition)
	{
	}
};