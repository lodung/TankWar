#ifndef PLAYERTANK_H
#define PLAYERTANK_H
#include <SDL.h>
#include <vector>
#include "wall.h"
#include "Bullet.h"
#include "constants.h"
#include "algorithm"

class PlayerTank {
public:
    int x, y;
    int dirX, dirY;
    SDL_Rect rect;
    std::vector<Bullet> bullets;

    PlayerTank(int startX, int startY);
    PlayerTank();

    void move(int dx, int dy, const std::vector<Wall>& walls);

    void shoot();
    void updateBullets();
    void render(SDL_Renderer* renderer);
};

#endif
