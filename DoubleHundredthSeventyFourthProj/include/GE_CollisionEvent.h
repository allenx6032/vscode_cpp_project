#pragma once

#include "GE_ECS.h"
#include "GE_Event.h"

class CollisionEvent : public Event
{
public:
	Entity m_a;
	Entity m_b;

	CollisionEvent(Entity a, Entity b) 
		: m_a(a)
		, m_b(b)
	{}
};