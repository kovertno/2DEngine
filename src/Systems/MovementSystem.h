#ifndef MOVEMENT_SYSTEM_H
#define MOVEMENT_SYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Events/CollisionEvent.h"

class MovementSystem : public System{
    private:

    public:
        MovementSystem(){
            RequireComponent<TransformComponent>();
            RequireComponent<RigidBodyComponent>();
            RequireComponent<SpriteComponent>();
        }

        ~MovementSystem() = default;

        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
            eventBus->SubscribeToEvent<CollisionEvent>(this, &MovementSystem::OnCollision);
        }

        void OnCollision(CollisionEvent& event) {
            Entity a = event.a;
            Entity b = event.b;

            if (a.BelongsToGroup("enemies") && b.BelongsToGroup("obstacles")) {
                OnEnemyHitsObstacle(a, b);
            }

            if (b.BelongsToGroup("obstacles") && a.BelongsToGroup("enemies")) {
                OnEnemyHitsObstacle(b, a);
            }
        }

        void OnEnemyHitsObstacle(Entity enemy, Entity obstacle) {
            if (enemy.HasComponent<RigidBodyComponent>()) {
                auto& rigidbody = enemy.GetComponent<RigidBodyComponent>();
                auto& sprite = enemy.GetComponent<SpriteComponent>();

                if (rigidbody.velocity.x != 0) {
                    rigidbody.velocity.x *= -1;
                    sprite.flip = (sprite.flip == SDL_FLIP_NONE) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
                }

                if (rigidbody.velocity.y != 0) {
                    rigidbody.velocity.y *= -1;
                    sprite.flip = (sprite.flip == SDL_FLIP_NONE) ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
                }
            }
        }

        void Update(double deltaTime){
            //Loop all entities that the system is interested in
            for(auto entity :  GetSystemEntities()){
                TransformComponent& transform = entity.GetComponent<TransformComponent>();
                const RigidBodyComponent rigidbody = entity.GetComponent<RigidBodyComponent>();

                transform.position.x += rigidbody.velocity.x * deltaTime;
                transform.position.y += rigidbody.velocity.y * deltaTime;


                if (entity.HasTag("player")) {
                    int paddingLeft = 0;
                    int paddingRight = 32;
                    int paddingTop = 0;
                    int paddingBottom = 32;

                    transform.position.x = transform.position.x < paddingLeft ? paddingLeft : transform.position.x;
                    transform.position.x = transform.position.x > Game::mapWidth - paddingRight ? Game::mapWidth - paddingRight : transform.position.x;
                    transform.position.y = transform.position.y < paddingTop ? paddingTop : transform.position.y;
                    transform.position.y = transform.position.y > Game::mapHeight - paddingBottom ? Game::mapHeight - paddingBottom : transform.position.y;
                }

                bool isEntityOutsideMap = (
                    transform.position.x < 0 ||
                    transform.position.x > Game::mapWidth ||
                    transform.position.y < 0 ||
                    transform.position.y > Game::mapHeight
                );

                if (isEntityOutsideMap && !entity.HasTag("player")) {
                    entity.Kill();
                }
            }
        }
};

#endif