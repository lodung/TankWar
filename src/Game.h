#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <fstream>
#include <algorithm>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "PlayerTank.h"
#include "EnemyTank.h"
#include "Wall.h"
#include "Explosion.h"
#include "Constants.h"

class Game {
public:
    bool isPause;
    bool lose;
    bool showingRanking = false;
    bool over = false;
    int demslg1;
    SDL_Texture* menuBackgroundTexture = nullptr;
    SDL_Texture* levelTextTexture;
    SDL_Texture* scoreTexture;
    SDL_Texture* hpTexture;
    SDL_Texture* hp2Texture;
    SDL_Texture* rankingTexture;
    SDL_Texture* baseHpTexture;
    SDL_Texture* enemySpawmTexture;
    SDL_Rect enemySpawnRect;
    SDL_Rect baseHpRect;
    SDL_Rect hpRect;
    SDL_Rect rankingRect;
    SDL_Rect hp2Rect;
    SDL_Rect scoreRect;
    SDL_Rect levelTextRect;
    Mix_Chunk* shootSound;
    Mix_Chunk* levelUpSound;
    Mix_Chunk* winSound;
    Mix_Chunk* gameOverSound;
    TTF_Font* font;
    TTF_Font* font2;
    TTF_Font* fontbrick;
    TTF_Font* fontLevelUp;
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
    std::vector<EnemyTank> enemies;
    int level;
    bool menu;
    int tocdo1,tocdo2;
    std::string dangcap;
    std::vector<int> highScores;
    std::vector<wallExplosion> wallExplosions;
    std::vector<Explosion> Explosions;
    int score;
    BossTank boss;
    bool bossActive = false;
    Game();
    ~Game();
    SDL_Point SPAWN_POINT = {1 * TILE_SIZE, 1 * TILE_SIZE};
    int enemiesSpawned;
    Uint32 lastSpawnTime;
    void handleMouseEvents(SDL_Event& event);
    bool isMouseInRect(int mouseX, int mouseY, const SDL_Rect& rect);
    void handleMenuSelection();
    void handleSubMenuSelection();
    void updateLevelDisplay();
    void updateScoreDisplay();
    void updateHpDisplay();
    void showLevelUpMessage();
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
    int selectedSubMenuOption;
    SDL_Texture* subMenuOptionsTexture[3];
    SDL_Rect subMenuOptionsRect[3];
    const char* subMenuOptions[3] = {"1 Player", "2 Players", "Back"};
    void resetGame();
    void showWinMessage();
    void showGameOverMessage();
    void saveScore();
    void loadScores();
    void showRanking();
    void updateRankingDisplay();
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
