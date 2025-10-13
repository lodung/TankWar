#ifndef PLAYERTANK_H
#define PLAYERTANK_H
#include <SDL.h>
#include <SDL_mixer.h>
#include <vector>
#include <string>
#include "Wall.h"
#include "Bullet.h"
#include "Constants.h"
#include "algorithm"

class PlayerTank {
public:
    int x, y;
    int dirX, dirY;
    double angle;
    int hp;
    bool active;
    SDL_Rect rect;
    std::vector<Bullet> bullets;
    std::vector<BigBullet> bigBullets;
    SDL_Texture *tankTexture;
    Mix_Chunk* shootSound;
    void calculateAngle();
    PlayerTank(int startX, int startY, SDL_Renderer *renderer,const std::string& texturePath,Mix_Chunk* sound);
    PlayerTank();
    void move(int dx, int dy, const std::vector<Wall>& walls, const std::vector<Stone>& stones);
    void shoot(SDL_Renderer* renderer);
    void shootBigBullet(SDL_Renderer* renderer);
    void updateBullets();
    void updateBigBullets();
    void render(SDL_Renderer* renderer);
};

#endif
