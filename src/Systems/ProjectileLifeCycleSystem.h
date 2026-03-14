#ifndef GAMEENGINE_PROJECTILELIFECYCLESYSTEM_H
#define GAMEENGINE_PROJECTILELIFECYCLESYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/ProjectileComponent.h"
#include <SDL.h>

class ProjectileLifeCycleSystem : public System {
    private:

    public:
        ProjectileLifeCycleSystem() {
            RequireComponent<ProjectileComponent>();
        }

        void Update() {
            for (auto entity : GetSystemEntities()) {
                const auto projectile = entity.GetComponent<ProjectileComponent>();

                // Kill projectiles after they reach their duration limit
                if (SDL_GetTicks() - projectile.startTime > projectile.duration) {
                    entity.Kill();
                }
            }
        }
};

#endif