#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <vector>
#include "PlayerTank.h"
#include "EnemyTank.h"
#include "Wall.h"
#include "Constants.h"

class Game {
public:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    std::vector<Wall> walls;
    PlayerTank player;
    int enemyNumber;
    std::vector<EnemyTank> enemies;
    Game();
    ~Game();
    void generateWalls();
    void spawnEnemies();
    void handleEvents();
    void update();
    void render();
    void run();
};

#endif
