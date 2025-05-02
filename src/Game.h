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

    SDL_Texture* levelTextTexture;
    SDL_Texture* scoreTexture;
    SDL_Rect scoreRect;
    SDL_Rect levelTextRect;
    Mix_Chunk* shootSound;
    TTF_Font* font;
    TTF_Font* font2;
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
    int level;
    bool menu;
    int tocdo1,tocdo2;
    std::string dangcap;
    int score;
    Game();
    ~Game();
    void updateLevelDisplay();
    void updateScoreDisplay();

    void showMenu();
    void updateMenuDisplay();
    int selectedOption;
    SDL_Texture* titleTexture ;
    SDL_Rect titleRect;
    SDL_Texture* optionsTexture[3];
    SDL_Rect optionsRect[3] ;
    int gameMode = 0;
    const char* menuOptions[3] = {"Start Game", "Ranking", "Exit"} ;

    bool subMenu = false;
    int selectedSubMenuOption = 0;
    SDL_Texture* subMenuOptionsTexture[3];
    SDL_Rect subMenuOptionsRect[3];
    const char* subMenuOptions[3] = {"1 Player", "2 Players", "Back"};

    void showSubMenu();
    void updateSubMenuDisplay();
    void loadMusic();
    void generateWalls();
    void spawnEnemies();
    void handleEvents();
    void update();
    void render();
    void run();
};

#endif
