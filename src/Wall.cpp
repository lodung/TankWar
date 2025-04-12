#include "Wall.h"
#include "Constants.h"
#include <SDL.h>
#include <SDL_image.h>
SDL_Texture *wallTexture;
SDL_Texture *stoneTexture;
Wall::Wall(int startX, int startY,SDL_Renderer *renderer) {
    x = startX;
    y = startY;
    active = true;
    wallTexture = IMG_LoadTexture(renderer,"image/wall.png");
    rect = {x, y, TILE_SIZEm, TILE_SIZEm};
}

void Wall::render(SDL_Renderer* renderer) {
    if (active) {
        SDL_RenderCopy(renderer, wallTexture, NULL,&rect);
    }
}
Stone::Stone(int startX, int startY,SDL_Renderer *renderer) {
    x = startX;
    y = startY;
    active = true;
    stoneTexture = IMG_LoadTexture(renderer,"image/stone.png");
    rect = {x, y, TILE_SIZEm, TILE_SIZEm};
}

void Stone::render(SDL_Renderer* renderer) {
    if (active) {
        SDL_RenderCopy(renderer, stoneTexture, NULL,&rect);
    }
}
