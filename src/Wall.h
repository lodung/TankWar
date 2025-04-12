#ifndef WALL_H
#define WALL_H

#include <SDL.h>
const int TILE_SIZEm = 20;
class Wall {
public:
    int x, y;
    SDL_Rect rect;
    bool active;

    Wall(int startX, int startY, SDL_Renderer *renderer );
    void render(SDL_Renderer* renderer);
};
class Stone {
public:
    int x,y;
    SDL_Rect rect;
    bool active;
    Stone(int startX, int startY, SDL_Renderer *renderer );
    void render(SDL_Renderer* renderer);
};
#endif
