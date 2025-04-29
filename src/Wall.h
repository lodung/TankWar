#ifndef WALL_H
#define WALL_H

#include <SDL.h>
class Wall {
public:
    int x, y;
    SDL_Rect rect;
    SDL_Texture *wallTexture;
    bool active;
    Wall(int startX, int startY, SDL_Renderer *renderer );
    void render(SDL_Renderer* renderer);
};
class Stone {
public:
    int x,y;
    SDL_Rect rect;
    SDL_Texture *stoneTexture;
    bool active;
    Stone(int startX, int startY, SDL_Renderer *renderer );
    void render(SDL_Renderer* renderer);
};
class Bush{
public:
    int x,y;
    SDL_Rect rect;
    SDL_Texture *bushTexture;
    Bush(int startX,int startY, SDL_Renderer *renderer);
    void render(SDL_Renderer* renderer);
};

class Ice{
public:
    int x,y;
    SDL_Rect rect;
    SDL_Texture *iceTexture;
    Ice(int startX,int startY, SDL_Renderer *renderer);
    void render(SDL_Renderer* renderer);
};
class Water{
public:
    int x,y;
    SDL_Rect rect;
    SDL_Texture *waterTexture;
    Water (int startX,int startY, SDL_Renderer *renderer);
    void render(SDL_Renderer* renderer);
};
class Base{
public:
    int x,y;
    SDL_Rect rect;
    SDL_Texture *baseTexture;
    bool active;
    Base(){};
    Base(int startX, int startY, SDL_Renderer* renderer);
    void render(SDL_Renderer *renderer);
};
#endif
