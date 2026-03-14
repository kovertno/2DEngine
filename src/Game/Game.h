#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include "../ECS/ECS.h"
#include "../AssetManager/AssetManager.h"
#include "../EventBus/EventBus.h"
#include <memory>
#include <sol/sol.hpp>

const int FPS = 60;
//1 second is 1000 miliseconds,
//so to get 30 frames per second we divide 1000 by 30 to know how many miliseconds a frame is displayed
const int MILLISECS_PER_FRAME = 1000/FPS; 

class Game{
    private:
        bool isRunning;
        bool isDebug = false;
        int millisecsPreviousFrame = 0;
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Rect camera;

        sol::state lua;

        std::unique_ptr<Registry> registry;
        std::unique_ptr<AssetManager> assetManager;
        std::unique_ptr<EventBus> eventBus;
        
    public:
        Game();
        ~Game();
        void Initialize();
        void Run();
        void Setup();
        void ProcessInput();
        void Update();
        void Render();
        void Destroy();

        static int windowWidth;
        static int windowHeight;
        static int mapWidth;
        static int mapHeight;
};

#endif