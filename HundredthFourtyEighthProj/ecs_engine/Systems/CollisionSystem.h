#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "Engine/ECS/ECS.h"
#include "Engine/EventBus/EventBus.h"
#include "Engine/Events/CollisionEvent.h"
#include "Engine/Components/BoxColliderComponent.h"
#include "Engine/Components/TransformComponent.h"

class CollisionSystem : public System {
    public:
        CollisionSystem() {
            RequireComponent<BoxColliderComponent>();
            RequireComponent<TransformComponent>();
        }

        void Update(std::unique_ptr<EventBus>& eventBus) {
            auto entities = GetSystemEntities();
            
            for (auto i = entities.begin(); i != entities.end(); i++) {
                Entity a = *i;
                auto aTransform = a.GetComponent<TransformComponent>();
                auto aCollider = a.GetComponent<BoxColliderComponent>();

                for (auto j = i; j != entities.end(); j++) {
                    Entity b = *j;

                    if (a == b) {
                        continue;
                    }

                    auto bTransform = b.GetComponent<TransformComponent>();
                    auto bCollider = b.GetComponent<BoxColliderComponent>();

                    bool collisionHappened = CheckAABBCollision(
                        aTransform.position.x + aCollider.offset.x,
                        aTransform.position.y + aCollider.offset.y,
                        aCollider.width,
                        aCollider.height,
                        bTransform.position.x + bCollider.offset.x,
                        bTransform.position.y + bCollider.offset.y,
                        bCollider.width,
                        bCollider.height
                    );

                    if (collisionHappened) {
                        Logger::Log("Entity " + std::to_string(a.GetId()) + " is colliding with Entity " + std::to_string(b.GetId()));

                        eventBus->EmitEvent<CollisionEvent>(a, b);
                    }
                }
            }
        }

        bool CheckAABBCollision(double aX, double aY, double aW, double aH, double bX, double bY, double bW, double bH) {
            return (
                aX < bX + bW &&
                aX + aW > bX &&
                aY < bY + bH &&
                aY + aH > bY
            );
        }
};

#endif
