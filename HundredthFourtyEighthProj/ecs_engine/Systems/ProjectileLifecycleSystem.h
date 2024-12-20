#ifndef PROJECTILELIFECYCLESYSTEM_H
#define PROJECTILELIFECYCLESYSTEM_H

#include "Engine/ECS/ECS.h"
#include "Engine/Components/ProjectileComponent.h"
#include <SDL.h>

class ProjectileLifecycleSystem : public System {
    public:
        ProjectileLifecycleSystem() {
            RequireComponent<ProjectileComponent>();
        }

        void Update() {
            for (auto entity: GetSystemEntities()) {
                auto projectile = entity.GetComponent<ProjectileComponent>();

                if (SDL_GetTicks() - projectile.startTime > projectile.duration) {
                    entity.Kill();
                }
            }
        }
};

#endif