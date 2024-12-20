#ifndef COLLISIONEVENT_H
#define COLLISIONEVENT_H

#include "Engine/ECS/ECS.h"
#include "Engine/EventBus/Event.h"

class CollisionEvent: public Event {
    public:
        Entity a;
        Entity b;
        CollisionEvent(Entity a, Entity b): a(a), b(b) {}
};

#endif
