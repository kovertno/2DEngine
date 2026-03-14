#include "AssetManager.h"
#include "../Logger/Logger.h"
#include <SDL_image.h>

AssetManager::AssetManager() {
    Logger::Log("AssetManager constructor called!");
}

AssetManager::~AssetManager() {
    ClearAssets();
    Logger::Log("AssetManager destructor called!");
}

void AssetManager::ClearAssets() {
    for(auto texture : textures) {
        SDL_DestroyTexture(texture.second);
    }
    textures.clear();

    //TODO: Clear all the fonts
    for (auto font : fonts) {
        TTF_CloseFont(font.second);
    }
    fonts.clear();
}

void AssetManager::AddTexture(SDL_Renderer* renderer, const std::string& assetId, const std::string& filePath) {
    SDL_Surface* surface = IMG_Load(filePath.c_str());
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    // Add the texture to the map
    textures.emplace(assetId, texture);

    Logger::Log("New texutre added to the Assed Manager with id = " + assetId);
}

SDL_Texture* AssetManager::GetTexture(const std::string& assetId) const {
    return textures.at(assetId);
}

void AssetManager::AddFont(const std::string &assetId, const std::string &filePath, int fontSize) {
    fonts.emplace(assetId, TTF_OpenFont(filePath.c_str(), fontSize));
}

TTF_Font* AssetManager::GetFont(const std::string& assetId) const {
    return fonts.at(assetId);
}
