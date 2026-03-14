#ifndef KEYBOARD_MOVEMENT_SYSTEM_H
#define KEYBOARD_MOVEMENT_SYSTEM_H 

#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/KeyPressedEvent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/KeyboardControlledComponent.h"

class KeyboardMovementSystem : public System {
    private:

    public:
        KeyboardMovementSystem() {
            RequireComponent<KeyboardControlledComponent>();
            RequireComponent<RigidBodyComponent>();
            RequireComponent<SpriteComponent>();
        }

        void OnKeyPressed(KeyPressedEvent& event) {

            for(auto entity : GetSystemEntities()) {
                const auto keyboardcontrol = entity.GetComponent<KeyboardControlledComponent>();
                auto& rigidbody = entity.GetComponent<RigidBodyComponent>();
                auto& sprite = entity.GetComponent<SpriteComponent>();
                switch(event.symbol) {
                    case SDLK_UP:
                        rigidbody.velocity = keyboardcontrol.upVelocity;
                        sprite.srcRect.y = sprite.height * 0;
                        break;
                    case SDLK_RIGHT:
                        rigidbody.velocity = keyboardcontrol.rightVelocity;
                        sprite.srcRect.y = sprite.height * 1;
                        break;
                    case SDLK_DOWN:
                        rigidbody.velocity = keyboardcontrol.downVelocity;
                        sprite.srcRect.y = sprite.height * 2;
                        break;
                    case SDLK_LEFT:
                        rigidbody.velocity = keyboardcontrol.leftVelocity;
                        sprite.srcRect.y = sprite.height * 3;
                        break;
                }

            }
            Logger::Log("KeyboardMovementSystem::OnKeyPressed - END");
        }

        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
            eventBus->SubscribeToEvent<KeyPressedEvent>(this, &KeyboardMovementSystem::OnKeyPressed);
        }
    
        void Update() {

        }
};

#endif