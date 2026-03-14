#ifndef LOAD_LEVEL_H
#define LOAD_LEVEL_H
#include "../ECS/ECS.h"
#include "../AssetManager/AssetManager.h"
#include <memory>
#include <sol/sol.hpp>

class LevelLoader {
    private:

    public:
        LevelLoader();
        ~LevelLoader();

        void LoadLevel(sol::state& lua, const std::unique_ptr<Registry>& registry, const std::unique_ptr<AssetManager>& assetManager, SDL_Renderer* renderer, int level);
};

#endif