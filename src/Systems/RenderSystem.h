#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include <SDL.h>
#include "../AssetManager/AssetManager.h"
#include "../ECS/ECS.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "../Logger/Logger.h"

class RenderSystem : public System{
    private:

    public:
        RenderSystem(){
            RequireComponent<TransformComponent>();
            RequireComponent<SpriteComponent>();
        }

        ~RenderSystem() = default;

        void Update(SDL_Renderer* renderer, std::unique_ptr<AssetManager>& assetManager, SDL_Rect& camera){
            // Create a vector with both Sprite and Transform components of all entities
            struct RenderableEntity {
                TransformComponent transformComponent;
                SpriteComponent spriteComponent;
            };

            std::vector<RenderableEntity> renderableEntities;
            for (auto entity : GetSystemEntities()) {
                RenderableEntity renderableEntity;
                renderableEntity.transformComponent = entity.GetComponent<TransformComponent>();
                renderableEntity.spriteComponent = entity.GetComponent<SpriteComponent>();

                // Bypass rendering entities outside of the camera
                bool isEntityInsideView = (
                    renderableEntity.transformComponent.position.x + (renderableEntity.spriteComponent.width * renderableEntity.transformComponent.scale.x) > camera.x &&
                    renderableEntity.transformComponent.position.x < camera.x + camera.w &&
                    renderableEntity.transformComponent.position.y + (renderableEntity.spriteComponent.height * renderableEntity.transformComponent.scale.y) > camera.y &&
                    renderableEntity.transformComponent.position.y < camera.y + camera.h
                );

                if (!isEntityInsideView && !renderableEntity.spriteComponent.isFixed) {
                    continue;
                }

                renderableEntities.push_back(renderableEntity);
            }

            // Sort all the entities of our system by z-index
            std::sort(renderableEntities.begin(), renderableEntities.end(), [](const RenderableEntity& a, const RenderableEntity& b){
                return a.spriteComponent.zIndex < b.spriteComponent.zIndex;
            });

            // Loop all entities that the system is interested in
            for(auto entity :  renderableEntities){
                const auto transform = entity.transformComponent;;
                const auto sprite = entity.spriteComponent;

                // Set the source rect and dest rect of out original texture
                SDL_Rect srcRect = sprite.srcRect;
                SDL_Rect dstRect = {
                    static_cast<int>(transform.position.x - (sprite.isFixed ? 0 : camera.x)),
                    static_cast<int>(transform.position.y - (sprite.isFixed ? 0 : camera.y)),
                    static_cast<int>(sprite.width * transform.scale.x),
                    static_cast<int>(sprite.height * transform.scale.y)
                };

                SDL_RenderCopyEx(
                    renderer,
                    assetManager->GetTexture(sprite.assetId),
                    &srcRect,
                    &dstRect,
                    transform.rotation,
                    NULL,
                    sprite.flip
                );
            }
        }
};

#endif