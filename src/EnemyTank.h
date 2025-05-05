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
    int dirX, dirY;
    double angle;
    int moveDelay;
    Uint32 lastShootTime = 600;
    Uint32 shootCooldown = 2000;
    SDL_Rect rect;
    SDL_Texture *enemyTexture;
    bool active;
    vector<Bullet> bullets;
    enum Direction { UP, DOWN, LEFT, RIGHT };
    Direction dir;
    EnemyTank(int startX, int startY,SDL_Renderer *renderer);
    void moveTowardPlayer(int playerX, int playerY, const vector<Wall>& walls,const vector<Stone>& stones);
    bool canMove(int newX, int newY, const vector<Wall>& walls, const vector<Stone>& stones);
    void applyMove(int dx, int dy);
    void calculateAngle();
    void shoot(SDL_Renderer* renderer);
    void updateBullets();
    void render(SDL_Renderer* renderer);
};
#endif // ENEMYTANK_H
