#ifndef KEY_PRESSED_EVENT_H
#define KEY_PRESSED_EVENT_H

#include "../EventBus/Event.h"
#include "SDL.h"
#include "../ECS/ECS.h"

class KeyPressedEvent : public Event {
    private:

    public:
        SDL_Keycode symbol;
        KeyPressedEvent(SDL_Keycode symbol) {
            this->symbol = symbol;
        }

};

#endif