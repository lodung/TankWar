#ifndef ENEMYTANK_H
#define ENEMYTANK_H
#include <SDL.h>
#include <vector>
#include "Bullet.h"
#include "Wall.h"
#include "Constants.h"

class EnemyTank {
public:
    int x, y;
    int dirX, dirY;
    int moveDelay, shootDelay;
    SDL_Rect rect;
    bool active;
    std::vector<Bullet> bullets;

    EnemyTank(int startX, int startY);
    void move(const std::vector<Wall>& walls);
    void shoot();
    void updateBullets();
    void render(SDL_Renderer* renderer);
};
#endif // ENEMYTANK_H
