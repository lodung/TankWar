#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "PlayerTank.h"
#include "EnemyTank.h"
#include "Wall.h"
#include "Constants.h"

class Game {
public:
    bool isPause;
    Mix_Chunk* shootSound;
    TTF_Font* font;
    SDL_Texture* startScreenTexture;
    Mix_Music* backGroundMusic;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* backgroundTexture;
    bool running;
    std::vector<Wall> walls;
    std::vector<Stone> stones;
    std::vector<Bush> bushs;
    std::vector<Ice> ices;
    std::vector<Water> waters;
    Base base;
    PlayerTank player;
    PlayerTank player2;
    int enemyNumber;
    std::vector<EnemyTank> enemies;
    int level; bool menu;
    int tocdo1,tocdo2;
    std::string dangcap;
    int score;
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
