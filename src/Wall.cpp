#include "Wall.h"
#include "Constants.h"
#include <SDL.h>
#include <SDL_image.h>




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


Bush::Bush(int startX, int startY,SDL_Renderer *renderer){
    x = startX;
    y = startY;
    bushTexture = IMG_LoadTexture(renderer,"image/bush.png");
    rect = {x, y, TILE_SIZEm, TILE_SIZEm};
}
void Bush::render(SDL_Renderer* renderer) {
        SDL_RenderCopy(renderer, bushTexture, NULL,&rect);
}

Ice::Ice(int startX, int startY, SDL_Renderer *renderer)
{
    x  = startX;
    y  = startY;
    iceTexture = IMG_LoadTexture(renderer,"image/ice.png");
    rect = {x,y,TILE_SIZEm, TILE_SIZEm};
}
void Ice::render(SDL_Renderer* renderer){
    SDL_RenderCopy(renderer, iceTexture, NULL, &rect);
}
Water::Water(int startX, int startY, SDL_Renderer *renderer)
{
    x  = startX;
    y  = startY;
    waterTexture = IMG_LoadTexture(renderer,"image/water.png");
    rect = {x,y,TILE_SIZEm, TILE_SIZEm};
}
void Water::render(SDL_Renderer* renderer){
    SDL_RenderCopy(renderer, waterTexture, NULL, &rect);
}
