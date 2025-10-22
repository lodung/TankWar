#ifndef ENEMYTANK_H
#define ENEMYTANK_H
#include <SDL.h>
#include <vector>
#include "Bullet.h"
#include "Wall.h"
#include "Constants.h"
using namespace std;
class EnemyTank {
public:
    int x, y;
    int hp;
    int dirX, dirY;
    double angle;
    int moveDelay;
    Uint32 lastShootTime = 600;
    Uint32 shootCooldown = 2000;
    SDL_Rect rect;
    SDL_Texture *enemyTexture;
    bool active;
    vector<Bullet> bullets;
    EnemyTank(int startX, int startY,SDL_Renderer *renderer, int health = 1);
    void moveTowardPlayer(int playerX, int playerY, const vector<Wall>& walls,const vector<Stone>& stones);
    bool canMove(int newX, int newY, const vector<Wall>& walls, const vector<Stone>& stones);
    void applyMove(int dx, int dy);
    void calculateAngle();
    void shoot(SDL_Renderer* renderer);
    void updateBullets();
    void render(SDL_Renderer* renderer);
};

class DemonTank {
public:
    int x, y;
    int hp;
    int dirX, dirY;
    double angle;
    int moveDelay;
    Uint32 lastShootTime = 0;
    Uint32 shootCooldown = 1000;
    SDL_Rect rect;
    SDL_Texture *demonTexture;
    bool active;
    vector<Bullet> bullets;

    DemonTank(int startX, int startY, SDL_Renderer* renderer, int health = 1);
    void moveTowardPlayer(int playerX, int playerY, const vector<Wall>& walls, const vector<Stone>& stones);
    bool canMove(int newX, int newY, const vector<Wall>& walls, const vector<Stone>& stones);
    void applyMove(int dx, int dy);
    void calculateAngle();
    void shoot(SDL_Renderer* renderer);
    void updateBullets();
    void render(SDL_Renderer* renderer);
};

class BossTank {
public:
    int x, y;
    int hp;
    double angle = 0.0;
    SDL_Rect rect;
    SDL_Texture* bossTexture;
    bool active;
    std::vector<BigBullet> bulletsS;
    std::vector<Bullet> bullets;
    Uint32 lastShootTime;
    Uint32 shootCooldown;
    Uint32 lastRotateTime = 0;
    BossTank(int startX, int startY, SDL_Renderer* renderer);
    BossTank();
    void shoot(SDL_Renderer* renderer);
    void updateBullets();
    void update();
    void render(SDL_Renderer* renderer);
};
#endif
