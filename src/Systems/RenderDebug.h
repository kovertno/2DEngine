#ifndef RENDER_DEBUG_H
#define RENDER_DEBUG_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/BoxColliderComponent.h"
#include <SDL.h>

class RenderDebug : public System {
    private:

    public:
        RenderDebug() {
            RequireComponent<TransformComponent>();
            RequireComponent<BoxColliderComponent>();
        }

        void Update(SDL_Renderer* renderer, SDL_Rect& camera) {
            for(auto entity : GetSystemEntities()) {
                const auto transform = entity.GetComponent<TransformComponent>();
                const auto boxCollider = entity.GetComponent<BoxColliderComponent>();
                
                SDL_Rect colliderRect = {static_cast<int>(transform.position.x + boxCollider.offset.x - camera.x),
                                         static_cast<int>(transform.position.y + boxCollider.offset.y - camera.y),
                                         static_cast<int>(boxCollider.width),
                                         static_cast<int>(boxCollider.height)};
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_RenderDrawRect(renderer, &colliderRect);
            }
        }
};

#endif