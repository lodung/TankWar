#ifndef BULLET_H
#define BULLET_H
#include <SDL.h>

class Bullet {
public:
    int x, y;
    int dx, dy;
    SDL_Rect rect;
    bool active;
    double angle;
    int bulletSpeed;
    void calculateAngle();
    Bullet() = default ;
    Bullet(int startX, int startY, int dirX, int dirY,SDL_Renderer* renderer);
    void move();
    void render(SDL_Renderer* renderer);
};

#endif // BULLET_H
