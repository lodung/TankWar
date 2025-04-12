#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <vector>
#include <SDL_mixer.h>
#include "PlayerTank.h"
#include "EnemyTank.h"
#include "Wall.h"
#include "Constants.h"

class Game {
public:
    Mix_Music* backGroundMusic;
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    std::vector<Wall> walls;
    std::vector<Stone> stones;
    PlayerTank player;
    int enemyNumber;
    std::vector<EnemyTank> enemies;
    Game();
    ~Game();
    void loadMusic();
    void generateWalls();
    void spawnEnemies();
    void handleEvents();
    void update();
    void render();
    void run();
};

#endif
