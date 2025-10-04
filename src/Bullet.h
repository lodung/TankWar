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

    Bullet() = default ;
    Bullet(int startX, int startY, int dirX, int dirY,SDL_Renderer* renderer);

    void calculateAngle();
    void move();
    void render(SDL_Renderer* renderer);

    SDL_Texture *bulletTexture = nullptr;
};

class BigBullet {
public:
    int x, y;
    int dx, dy;
    SDL_Rect rect;
    bool active;
    double angle;
    int bigBulletSpeed;
    BigBullet() = default;
    BigBullet(int startX, int startY, int dirX, int dirY, SDL_Renderer* renderer);
    void calculateAngle();
    void move();
    void render(SDL_Renderer* renderer);
    SDL_Texture* bigBulletTexture = nullptr;
};

#endif // BULLET_H
