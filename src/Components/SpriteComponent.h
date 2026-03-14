#ifndef SPRITE_COMPONENT_H
#define SPRITE_COMPONENT_H

#include <glm/glm.hpp>
#include <string>
#include <SDL.h>

struct SpriteComponent{
    std::string assetId;
    double width;
    double height;
    unsigned int zIndex;
    bool isFixed;
    SDL_RendererFlip flip;
    SDL_Rect srcRect;

    SpriteComponent(std::string assetId = "", double width = 0, double height = 0, int zIndex = 0, bool isFixed = false, int srcRectX = 0, int srcRectY = 0) {
        this->assetId = assetId;
        this->width = width;
        this->height = height;
        this->zIndex = zIndex;
        this->isFixed = isFixed;
        this->flip = SDL_FLIP_NONE;
        this->srcRect = {srcRectX, srcRectY, static_cast<int>(width), static_cast<int>(height)};
    }
};

#endif