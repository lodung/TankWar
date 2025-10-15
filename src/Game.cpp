#include "Game.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <SDL_mixer.h>
#include <ctime>
Game::Game() {
    running = true;
 //   window = nullptr;
 //   renderer = nullptr;
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        running = false;
        return;
    }
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! Error: " << TTF_GetError() << std::endl;
        running = false;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! Error: " << Mix_GetError() << std::endl;
        running = false;
        return;
    }
    window = SDL_CreateWindow("Tank War", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        running = false;
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        running = false;
    }
    backGroundMusic = Mix_LoadMUS("sound/CHAMP.mp3");
    if (!backGroundMusic) {
        std::cerr << "Failed to load music! Error: " << Mix_GetError() << std::endl;
        running = false;
    } else {
        Mix_PlayMusic(backGroundMusic, -1);
    }
    font = TTF_OpenFont("ARIAL.ttf", 30);
    if (!font) {
        std::cerr << "Failed to load font! Error: " << TTF_GetError() << std::endl;
        running = false;
    }
    font2 = TTF_OpenFont("font.ttf", 100);
    if (!font2) {
        std::cerr << "Failed to load font! Error: " << TTF_GetError() << std::endl;
        running = false;
    }
    fontbrick = TTF_OpenFont("brick.ttf", 100);
    if (!fontbrick) {
        std::cerr << "Failed to load font! Error: " << TTF_GetError() << std::endl;
        running = false;
    }
    fontLevelUp = TTF_OpenFont("levelup.ttf", 50);
    if (!fontLevelUp) {
    std::cerr << "Failed to load level up font! Error: " << TTF_GetError() << std::endl;
    running = false;
    }
    shootSound = Mix_LoadWAV("sound/shoot.mp3");
    if (!shootSound) {
        std::cerr << "Failed to load shoot sound! Error: " << Mix_GetError() << std::endl;
        running = false;
    }
    gameOverSound = Mix_LoadWAV("sound/GameOver.mp3");
    if (!gameOverSound) {
        std::cerr << "Failed to load gameover sound! Error: " << Mix_GetError() << std::endl;
        running = false;
    }
    levelUpSound = Mix_LoadWAV("sound/levelup.mp3");
    if (!gameOverSound) {
        std::cerr << "Failed to load levelup sound! Error: " << Mix_GetError() << std::endl;
        running = false;
    }
    winSound = Mix_LoadWAV("sound/win.mp3");
    if (!winSound) {
        std::cerr << "Failed to load levelup sound! Error: " << Mix_GetError() << std::endl;
        running = false;
    }
    menuBackgroundTexture = IMG_LoadTexture(renderer, "image/background.png");
    if (!menuBackgroundTexture) {
        std::cerr << "Failed to load menu background image! " << IMG_GetError() << std::endl;
    }
    SDL_Surface* hpBarSurface = SDL_CreateRGBSurface(0, 20, 20, 32, 0, 0, 0, 0);
    SDL_FillRect(hpBarSurface, NULL, SDL_MapRGB(hpBarSurface->format, 255, 0, 0)); // Màu đỏ
    bossHpBarTexture = SDL_CreateTextureFromSurface(renderer, hpBarSurface);
    SDL_FreeSurface(hpBarSurface);
    if (!bossHpBarTexture) {
        std::cerr << "Failed to create boss hp bar texture!" << std::endl;
    }
    Mix_VolumeMusic(60);
    menu = true;
    isPause = false;
    over = false;
    subMenu = false;
    showingRanking = false;
    showingSettings = false;
    gameMode = 0;
    selectedOption = 0;
    selectedSubMenuOption = 0;
    selectedSettingsOption = 0;
    srand(time(0));
    for(int i = 0; i < 4; ++i) optionsTexture[i] = nullptr;
    for(int i = 0; i < 3; ++i) subMenuOptionsTexture[i] = nullptr;
    for(int i = 0; i < 3; ++i) settingsOptionsTexture[i] = nullptr;

    updateMenuDisplay();
    updateSubMenuDisplay();
    updateSettingsMenuDisplay();
}
void Game::resetGame() {
    over = false;
    menu = false;
    subMenu = false;
    isPause = false;
    showingSettings = false;
    showingGameModeMenu = false;
    score = 0;
    level = 1;
    demslg1 = 10;
    enemiesSpawned = 0;
    lastSpawnTime = 0;
    bossActive = false;
    boss.active = true;
    player = PlayerTank(5 * TILE_SIZE, 13 * TILE_SIZE, renderer, "image/Tank.png", shootSound);
    player.hp = 3;
    player2 = PlayerTank(9 * TILE_SIZE, 13 * TILE_SIZE, renderer, "image/Player2.png", shootSound);
    player2.hp = 3;
    base = Base(7 * TILE_SIZE, 13 * TILE_SIZE, renderer);
    base.hp = 1;
    base.active = true;
    tocdo1 = 4;
    tocdo2 = 4;
    enemies.clear();
    wallExplosions.clear();
    walls.clear();
    stones.clear();
    bushs.clear();
    waters.clear();
    ices.clear();
    dangcap = std::to_string(level);
    generateWalls();
    if (level == 36) {
        bossActive = true;

        boss = BossTank(
            SCREEN_WIDTH / 2 - TILE_SIZE*5,
            SCREEN_HEIGHT / 2 - TILE_SIZE*3,
            renderer
        );
        //        boss.hp = 30;
        boss.hp = isHardMode ? 30 : 20;
        updateBossHpDisplay();
    } else {
    spawnEnemies();
    }
    updateLevelDisplay();
    updateScoreDisplay();
    updateHpDisplay();
}

void Game::generateWalls() {
    walls.clear();
    bushs.clear();
    waters.clear();
    ices.clear();
    stones.clear();
    std::vector <std::vector <char>> Map(28,std::vector<char>(27,'.'));
    std::string timkiem;
    if (level == 36) {
        timkiem = "map/bossLevel.txt";
    } else {
        timkiem = "map/" + dangcap + ".txt";
    }
    std::ifstream file(timkiem);
    for(int i= 1; i<=27; i++)
    {
        for (int j = 1; j<=26; j++) {
            file >> Map[i][j];
        }
    }
    file.close();
    for (int i = 1; i<=27; i++) {
        for(int j = 0; j<=26 ;j++) {
            if(Map[i][j] == '#'){
                Wall w = Wall((j+1)*TILE_SIZEm,(i+1)*TILE_SIZEm,renderer);
                walls.push_back(w);
            }
            if(Map[i][j] == '@'){
                Stone k = Stone((j+1)*TILE_SIZEm,(i+1)*TILE_SIZEm,renderer);
                stones.push_back(k);
            }
            if(Map[i][j] == '%'){
                Bush k = Bush((j+1)*TILE_SIZEm,(i+1)*TILE_SIZEm,renderer);
                bushs.push_back(k);
            }
            if(Map[i][j] == '-'){
                Ice k = Ice((j+1)*TILE_SIZEm,(i+1)*TILE_SIZEm,renderer);
                ices.push_back(k);
            }
            if(Map[i][j] == '~'){
                Water k = Water((j+1)*TILE_SIZEm,(i+1)*TILE_SIZEm,renderer);
                waters.push_back(k);
            }
        }
    }

}

void Game::spawnEnemies() {
    if (level == 36) return;
    if (enemies.size() < 3 && enemiesSpawned < 10 && SDL_GetTicks() - lastSpawnTime >= 3000)
    {
        int spawnSide = rand() % 2; // 0: Góc trái, 1: Góc phải

        SDL_Point spawnPoint;
        if (spawnSide == 0) {
            spawnPoint = {1 * TILE_SIZE, 1 * TILE_SIZE};
        } else {
            spawnPoint = {13 * TILE_SIZE, 1 * TILE_SIZE};
        }
        int enemyHp = isHardMode ? 2 : 1;
        enemies.push_back(EnemyTank(spawnPoint.x, spawnPoint.y, renderer, enemyHp));
        enemiesSpawned++;
        lastSpawnTime = SDL_GetTicks();
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }

        handleMouseEvents(event);

        // PAUSE
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_z) { isPause = !isPause; }
        else if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE){ isPause = !isPause; }
        else if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q){ over = true; }
    }

    const Uint8* keystates = SDL_GetKeyboardState(nullptr); // Lấy trạng thái của tất cả các phím
    if(!isPause){
        if (player.active){
            if (keystates[SDL_SCANCODE_UP]) { player.move(0, -tocdo1, walls, stones); }
            if (keystates[SDL_SCANCODE_DOWN]) { player.move(0, tocdo1, walls, stones); }
            if (keystates[SDL_SCANCODE_LEFT]) { player.move(-tocdo1, 0, walls, stones); }
            if (keystates[SDL_SCANCODE_RIGHT]) { player.move(tocdo1, 0, walls, stones); }
            if (keystates[SDL_SCANCODE_SPACE]) { player.shoot(renderer); }
        }
        if(player2.active){
            if (keystates[SDL_SCANCODE_W]) { player2.move(0, -tocdo2, walls, stones); }
            if (keystates[SDL_SCANCODE_S]) { player2.move(0, tocdo2, walls, stones); }
            if (keystates[SDL_SCANCODE_A]) { player2.move(-tocdo2, 0, walls, stones); }
            if (keystates[SDL_SCANCODE_D]) { player2.move(tocdo2, 0, walls, stones); }
            if (keystates[SDL_SCANCODE_L]) { player2.shoot(renderer); }
        }
    }
}

 //Xử lý thao tác chuột.
void Game::handleMouseEvents(SDL_Event& event) {
    int mouseX, mouseY;

    switch (event.type) {
        case SDL_MOUSEMOTION:
            SDL_GetMouseState(&mouseX, &mouseY);

            // Xử lý hover trong menu chính
            if (menu) {
                int oldOption = selectedOption;
                for (int i = 0; i < 5; i++) {
                    if (isMouseInRect(mouseX, mouseY, optionsRect[i])) {
                        selectedOption = i;
                        break;
                    }
                }
                if (oldOption != selectedOption) updateMenuDisplay();
            }
            // Xử lý hover trong submenu
            else if (subMenu) {
                int oldOption = selectedSubMenuOption;
                for (int i = 0; i < 3; i++) {
                    if (isMouseInRect(mouseX, mouseY, subMenuOptionsRect[i])) {
                        selectedSubMenuOption = i;
                        break;
                    }
                }
                if (oldOption != selectedSubMenuOption) {
                    updateSubMenuDisplay();
                }
            }
            else if (showingGameModeMenu) {
                int oldOption = selectedGameModeMenuOption;
                for (int i = 0; i < 3; i++) {
                    if (isMouseInRect(mouseX, mouseY, gameModeMenuOptionsRect[i])) {
                        selectedGameModeMenuOption = i;
                        break;
                    }
                }
                if (oldOption != selectedGameModeMenuOption) updateGameModeMenuDisplay();
            }
            else if (showingSettings) {
                int oldOption = selectedSettingsOption;
                for (int i = 0; i < 3; i++) {
                    if (isMouseInRect(mouseX, mouseY, settingsOptionsRect[i])) {
                        selectedSettingsOption = i;
                        break;
                    }
                }
                if (oldOption != selectedSettingsOption) updateSettingsMenuDisplay();
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT) {
                SDL_GetMouseState(&mouseX, &mouseY);

                // Xử lý click trong menu chính
                if (menu) {
                     for (int i = 0; i < 5; i++) {
                        if (isMouseInRect(mouseX, mouseY, optionsRect[i])) {
                            selectedOption = i;
                            handleMenuSelection();
                            break;
                        }
                    }
                }
                // Xử lý click trong submenu
                else if (subMenu) {
                    for (int i = 0; i < 3; i++) {
                        if (isMouseInRect(mouseX, mouseY, subMenuOptionsRect[i])) {
                            selectedSubMenuOption = i;
                            handleSubMenuSelection();
                            break;
                        }
                    }
                }
                else if (showingGameModeMenu) {
                    for (int i = 0; i < 3; i++) {
                        if (isMouseInRect(mouseX, mouseY, gameModeMenuOptionsRect[i])) {
                            selectedGameModeMenuOption = i;
                            handleGameModeMenuSelection();
                            break;
                        }
                    }
                }
                // Xử lý click trong ranking screen
                else if (showingSettings) {
                    for (int i = 0; i < 3; i++) {
                        if (isMouseInRect(mouseX, mouseY, settingsOptionsRect[i])) {
                            selectedSettingsOption = i;
                            handleSettingsMenuSelection();
                            break;
                        }
                    }
                }
                else if (showingRanking) {
                    showingRanking = false;
                    menu = true;
                    updateMenuDisplay();
                }
            }
            break;
    }
}
bool Game::isMouseInRect(int mouseX, int mouseY, const SDL_Rect& rect) {
    return (mouseX >= rect.x &&
            mouseX <= rect.x + rect.w &&
            mouseY >= rect.y &&
            mouseY <= rect.y + rect.h);
}

void Game::handleMenuSelection() {
    switch (selectedOption) {
        case 0: // Start Game
            menu = false;
            subMenu = true;
            selectedSubMenuOption = 0;
            updateSubMenuDisplay();
            break;
        case 1: // Game Mode
            menu = false;
            showingGameModeMenu = true;
            selectedGameModeMenuOption = 0;
            updateGameModeMenuDisplay();
            break;
        case 2: // Settings
            menu = false;
            showingSettings = true;
            selectedSettingsOption = 0;
            updateSettingsMenuDisplay();
            break;
        case 3: // Ranking
            showingRanking = true;
            menu = false;
            loadScores();
            updateRankingDisplay();
            break;
        case 4: // Exit
            running = false;
            menu = false;
            break;
    }
}
void Game::handleSubMenuSelection() {
    switch (selectedSubMenuOption) {
        case 0: // 1 Player
            resetGame();
            gameMode = 1;
            subMenu = false;
            player2.active = false;
            player2.rect = {0,0,0,0};
            break;
        case 1: // 2 Players
            resetGame();
            gameMode = 2;
            subMenu = false;
            break;
        case 2: // Back
            subMenu = false;
            menu = true;
            updateMenuDisplay();
            break;
    }
}
void Game::handleSettingsMenuSelection() {
    switch (selectedSettingsOption) {
        case 0:
            if (Mix_PausedMusic() == 1) {
                Mix_ResumeMusic();
            } else {
                Mix_PauseMusic();
            }
            std::cout << "Music ON/OFF." << std::endl;
            break;
        case 1:
            isHardMode = !isHardMode;
            if (isHardMode) {
                settingsOptions[1] = "Difficulty: Hard";
            } else {
                settingsOptions[1] = "Difficulty: Normal";
            }
            updateSettingsMenuDisplay();
            break;
        case 2:
            showingSettings = false;
            menu = true;
            updateMenuDisplay();
            break;
    }
}
void Game::handleGameModeMenuSelection() {
    switch (selectedGameModeMenuOption) {
        case 0: // PvP
            std::cout << "PvP mode is still under development." << std::endl;
            break;
        case 1: // Creative
            std::cout << "Creative mode is still under development." << std::endl;
            break;
        case 2: // Back
            showingGameModeMenu = false;
            menu = true;
            updateMenuDisplay();
            break;
    }
}

/////////////////////////////////////
////Các thông báo trong trò chơi////
///////////////////////////////////
void Game::updateLevelDisplay() {
    std::string strCapDo;
    if (level == 36) {
        strCapDo = "LEVEL: BOSS";
    } else {
        strCapDo = "LEVEL: " + std::to_string(level);
    }
    SDL_Color textColor = {255, 255, 0}; // Màu vàng
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, strCapDo.c_str(), textColor);
    levelTextTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    levelTextRect = {SCREEN_WIDTH - 200, 40, textSurface->w, textSurface->h}; // Góc phải trên
    SDL_FreeSurface(textSurface);
}
void Game::updateScoreDisplay() {
    std::string strDiemSo = "SCORE " + std::to_string(score);
    SDL_Color textColor = {173, 216, 230};
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, strDiemSo.c_str(),textColor);
    scoreTexture = SDL_CreateTextureFromSurface(renderer,scoreSurface);
    scoreRect = {SCREEN_WIDTH - 200, 70, scoreSurface->w, scoreSurface->h};
    SDL_FreeSurface(scoreSurface);
}
void Game::updateHpDisplay(){
        std::string strHp1 = "HP I: " + std::to_string(player.hp);
        SDL_Color textColor = {0, 0 ,0};
        SDL_Surface* hpSurface = TTF_RenderText_Solid(font, strHp1.c_str(),textColor);
        hpTexture = SDL_CreateTextureFromSurface(renderer,hpSurface);
        hpRect = {SCREEN_WIDTH - 200, 120, hpSurface->w, hpSurface->h};
        SDL_FreeSurface(hpSurface);
    if (gameMode == 2){
        std::string strHp2 = "HP II: " + std::to_string(player2.hp);
        SDL_Surface* hp2Surface = TTF_RenderText_Solid(font, strHp2.c_str(),textColor);
        hp2Texture = SDL_CreateTextureFromSurface(renderer,hp2Surface);
        hp2Rect = {SCREEN_WIDTH - 200, 150, hp2Surface->w, hp2Surface->h};
        SDL_FreeSurface(hp2Surface);
    }
    std::string strBaseHp = "Base HP: " + std::to_string(base.hp);
    SDL_Surface* baseSurface = TTF_RenderText_Solid(font, strBaseHp.c_str(), textColor);
    baseHpTexture = SDL_CreateTextureFromSurface(renderer, baseSurface);
    baseHpRect = {SCREEN_WIDTH - 200, 190, baseSurface->w, baseSurface->h};
    SDL_FreeSurface(baseSurface);
    SDL_Color text2Color = {255, 50, 50};
    std::string strEnemy = "Enemies: " + std::to_string(demslg1);
    SDL_Surface* enemySurface = TTF_RenderText_Solid(font, strEnemy.c_str(),text2Color);
    enemySpawmTexture = SDL_CreateTextureFromSurface(renderer, enemySurface);
    enemySpawnRect = {SCREEN_WIDTH - 200, 230, enemySurface->w, enemySurface->h};
    SDL_FreeSurface(enemySurface);
}
void Game::updateBossHpDisplay() {
    if (bossHpLabelTexture) {
        SDL_DestroyTexture(bossHpLabelTexture);
        bossHpLabelTexture = nullptr;
    }
    SDL_Color textColor = {255, 127, 80};
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, "BOSS HP:", textColor);
    if (textSurface) {
        bossHpLabelTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        bossHpLabelRect = {10, 10, textSurface->w, textSurface->h};
        SDL_FreeSurface(textSurface);
    }
}

void Game::showLevelUpMessage() {
    Mix_PauseMusic();
    Mix_PlayChannel(-1, levelUpSound, 0);
    SDL_Color textColor = {255, 255, 0}; // Màu vàng
    SDL_Surface* textSurface = TTF_RenderText_Solid(fontLevelUp, "LEVEL UP!", textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_Rect textRect = {
        SCREEN_WIDTH / 2 - textSurface->w / 2,
        SCREEN_HEIGHT / 2 - textSurface->h / 2,
        textSurface->w,
        textSurface->h
    };

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Màu nền đen
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_RenderPresent(renderer);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    SDL_Delay(1500);
    Mix_ResumeMusic();
}
void Game::showGameOverMessage() {
    Mix_PauseMusic();
    Mix_PlayChannel(-1, gameOverSound, 0);
    SDL_Color gameOverColor = {255, 0, 0}; // Màu đỏ
    SDL_Surface* gameOverSurface = TTF_RenderText_Solid(fontLevelUp, "GAME OVER", gameOverColor);
    SDL_Texture* gameOverTexture = SDL_CreateTextureFromSurface(renderer, gameOverSurface);

    // Vị trí hiển thị thông báo "GAME OVER" ở giữa màn hình
    SDL_Rect gameOverRect = {
        SCREEN_WIDTH / 2 - gameOverSurface->w / 2,
        SCREEN_HEIGHT / 2 - gameOverSurface->h / 2,
        gameOverSurface->w,
        gameOverSurface->h
    };

    SDL_Color infoColor = {255, 255, 255}; // Màu trắng
    SDL_Surface* infoSurface = TTF_RenderUTF8_Solid(font, "Lưu điểm và quay trở lại menu chính", infoColor);
    SDL_Texture* infoTexture = SDL_CreateTextureFromSurface(renderer, infoSurface);
    SDL_Rect infoRect = {SCREEN_WIDTH / 2 - infoSurface->w / 2, SCREEN_HEIGHT - infoSurface->h - 50,
        infoSurface->w,
        infoSurface->h
    };
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Màu nền đen
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, gameOverTexture, NULL, &gameOverRect);
    SDL_RenderCopy(renderer, infoTexture, NULL, &infoRect);

    SDL_RenderPresent(renderer);

    SDL_FreeSurface(gameOverSurface);
    SDL_DestroyTexture(gameOverTexture);
    SDL_FreeSurface(infoSurface);
    SDL_DestroyTexture(infoTexture);

    SDL_Delay(2900);
    Mix_ResumeMusic();
}
void Game::showWinMessage() {
    Mix_PauseMusic();
    Mix_VolumeChunk(winSound, 128);
    Mix_PlayChannel(-1, winSound, 0);
    SDL_Color winColor = {0, 255, 0}; // Màu xanh lá
    SDL_Surface* winSurface = TTF_RenderUTF8_Solid(fontLevelUp, "YOU WIN!", winColor);
    SDL_Texture* winTexture = SDL_CreateTextureFromSurface(renderer, winSurface);

    // Vị trí hiển thị thông báo "YOU WIN!" ở giữa màn hình
    SDL_Rect winRect = {
        SCREEN_WIDTH / 2 - winSurface->w / 2,
        SCREEN_HEIGHT / 2 - winSurface->h / 2 - 50,
        winSurface->w,
        winSurface->h
    };

    SDL_Color infoColor = {255, 255, 255}; // Màu trắng
    SDL_Surface* infoSurface = TTF_RenderUTF8_Solid(font, "Chúc mừng bạn đã chiến thắng và hoàn tất hết màn chơi!", infoColor);
    SDL_Texture* infoTexture = SDL_CreateTextureFromSurface(renderer, infoSurface);
    SDL_Rect infoRect = {
        SCREEN_WIDTH / 2 - infoSurface->w / 2,
        winRect.y + winRect.h + 20,
        infoSurface->w,
        infoSurface->h
    };
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Màu nền đen
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, winTexture, NULL, &winRect);
    SDL_RenderCopy(renderer, infoTexture, NULL, &infoRect);

    SDL_RenderPresent(renderer);

    // Giải phóng tài nguyên
    SDL_FreeSurface(winSurface);
    SDL_DestroyTexture(winTexture);
    SDL_FreeSurface(infoSurface);
    SDL_DestroyTexture(infoTexture);

    SDL_Delay(4500);
    Mix_ResumeMusic();
}
void Game::updateMenuDisplay() {
    SDL_Color titleColor = {255, 0, 0};
    SDL_Surface* titleSurface = TTF_RenderText_Solid(fontbrick, "TANKWAR", titleColor);
    titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    titleRect = {SCREEN_WIDTH / 2 - titleSurface->w / 2, 100, titleSurface->w, titleSurface->h};
    SDL_FreeSurface(titleSurface);

    // Tạo texture cho các tùy chọn menu
    for (int i = 0; i < 5; i++) {
        SDL_Color optionColor;
        if (i == selectedOption) {
            optionColor = {255, 255, 0}; // Màu vàng cho tùy chọn được chọn
        } else {
            optionColor = {255, 255, 255}; // Màu trắng cho các tùy chọn khác
        }

        SDL_Surface* optionSurface = TTF_RenderText_Solid(font, menuOptions[i], optionColor);
        if (optionsTexture[i]) SDL_DestroyTexture(optionsTexture[i]);
        optionsTexture[i] = SDL_CreateTextureFromSurface(renderer, optionSurface);
        optionsRect[i] = {SCREEN_WIDTH / 2 - optionSurface->w / 2, 250 + i * 60, optionSurface->w, optionSurface->h};
        SDL_FreeSurface(optionSurface);
    }
}
void Game::updateSubMenuDisplay() {
    for (int i = 0; i < 3; i++) {
        SDL_Color optionColor;
        if (i == selectedSubMenuOption) {
            optionColor = {255, 255, 0}; // Màu vàng cho tùy chọn được chọn
        } else {
            optionColor = {255, 255, 255}; // Màu trắng cho các tùy chọn khác
        }

        SDL_Surface* optionSurface = TTF_RenderText_Solid(font, subMenuOptions[i], optionColor);
        if (optionSurface) {
             // Giải phóng texture cũ nếu có trước khi tạo mới
             if (subMenuOptionsTexture[i]) {
                 SDL_DestroyTexture(subMenuOptionsTexture[i]);
             }
             subMenuOptionsTexture[i] = SDL_CreateTextureFromSurface(renderer, optionSurface);
             subMenuOptionsRect[i] = {SCREEN_WIDTH / 2 - optionSurface->w / 2, 250 + i * 70, optionSurface->w, optionSurface->h}; // Vị trí tương tự menu chính
             SDL_FreeSurface(optionSurface);
        }
    }
}
void Game::updateSettingsMenuDisplay() {
    for (int i = 0; i < 3; i++) {
        SDL_Color optionColor = (i == selectedSettingsOption) ? SDL_Color{255, 255, 0} : SDL_Color{255, 255, 255};

        SDL_Surface* optionSurface = TTF_RenderText_Solid(font, settingsOptions[i], optionColor);
        if (optionSurface) {
             if (settingsOptionsTexture[i]) SDL_DestroyTexture(settingsOptionsTexture[i]);
             settingsOptionsTexture[i] = SDL_CreateTextureFromSurface(renderer, optionSurface);
             settingsOptionsRect[i] = {SCREEN_WIDTH / 2 - optionSurface->w / 2, 250 + i * 70, optionSurface->w, optionSurface->h};
             SDL_FreeSurface(optionSurface);
        }
    }
}
void Game::updateGameModeMenuDisplay() {
    for (int i = 0; i < 3; i++) {
        SDL_Color optionColor = (i == selectedGameModeMenuOption) ? SDL_Color{255, 255, 0} : SDL_Color{255, 255, 255};

        SDL_Surface* optionSurface = TTF_RenderText_Solid(font, gameModeMenuOptions[i], optionColor);
        if (optionSurface) {
             if (gameModeMenuOptionsTexture[i]) SDL_DestroyTexture(gameModeMenuOptionsTexture[i]);
             gameModeMenuOptionsTexture[i] = SDL_CreateTextureFromSurface(renderer, optionSurface);
             gameModeMenuOptionsRect[i] = {SCREEN_WIDTH / 2 - optionSurface->w / 2, 250 + i * 70, optionSurface->w, optionSurface->h};
             SDL_FreeSurface(optionSurface);
        }
    }
}

void Game::showMenu() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
            menu = false;
        }
        else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                    selectedOption = (selectedOption - 1 + 5) % 5;
                    updateMenuDisplay();
                    break;
                case SDLK_DOWN:
                    selectedOption = (selectedOption + 1) % 5;
                    updateMenuDisplay();
                    break;
                case SDLK_RETURN:
                    handleMenuSelection();
                    break;

            }
        }
        handleMouseEvents(event);
    }
    SDL_Rect bgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    if (menuBackgroundTexture) {
        SDL_RenderCopy(renderer, menuBackgroundTexture, NULL, &bgRect);
    } else {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
    }
    // Vẽ title, options, viền...
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
    for (int i = 0; i < 5; i++) {
        if(optionsTexture[i]) SDL_RenderCopy(renderer, optionsTexture[i], NULL, &optionsRect[i]);
    }
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_Rect selectedRect = {
        optionsRect[selectedOption].x - 20,
        optionsRect[selectedOption].y - 5,
        optionsRect[selectedOption].w + 40,
        optionsRect[selectedOption].h + 10
    };
    SDL_RenderDrawRect(renderer, &selectedRect);
    SDL_RenderPresent(renderer);
}
void Game::showSubMenu() {
    // Xử lý sự kiện menu con
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
            subMenu = false;
        }
        else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                    selectedSubMenuOption = (selectedSubMenuOption - 1 + 3) % 3;
                    updateSubMenuDisplay();
                    break;
                case SDLK_DOWN:
                    selectedSubMenuOption = (selectedSubMenuOption + 1) % 3;
                    updateSubMenuDisplay();
                    break;
                case SDLK_RETURN: // Phím Enter
                    handleSubMenuSelection();
                    break;
                 case SDLK_ESCAPE: // Phím Escape cũng có thể dùng để Back
                    subMenu = false;
                    menu = true;
                    updateMenuDisplay();
                    break;
            }
        }
        handleMouseEvents(event);
    }

    // Render menu con
    SDL_Rect bgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    if (menuBackgroundTexture) {
        SDL_RenderCopy(renderer, menuBackgroundTexture, NULL, &bgRect);
    } else {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
    }

    // Vẽ các tùy chọn menu con
    for (int i = 0; i < 3; i++) {
         if (subMenuOptionsTexture[i]) {
            SDL_RenderCopy(renderer, subMenuOptionsTexture[i], NULL, &subMenuOptionsRect[i]);
         }
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Màu vàng cho viền
     if (subMenuOptionsTexture[selectedSubMenuOption]) { // Chỉ vẽ nếu texture tương ứng hợp lệ
        SDL_Rect selectedRectHighlight = {
            subMenuOptionsRect[selectedSubMenuOption].x - 20,
            subMenuOptionsRect[selectedSubMenuOption].y - 5,
            subMenuOptionsRect[selectedSubMenuOption].w + 40,
            subMenuOptionsRect[selectedSubMenuOption].h + 10
        };
        SDL_RenderDrawRect(renderer, &selectedRectHighlight);
     }

    SDL_RenderPresent(renderer);
}
void Game::showSettingsMenu() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
            showingSettings = false;
        }
        else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                    selectedSettingsOption = (selectedSettingsOption - 1 + 3) % 3;
                    updateSettingsMenuDisplay();
                    break;
                case SDLK_DOWN:
                    selectedSettingsOption = (selectedSettingsOption + 1) % 3;
                    updateSettingsMenuDisplay();
                    break;
                case SDLK_RETURN:
                    handleSettingsMenuSelection();
                    break;
                 case SDLK_ESCAPE:
                    showingSettings = false;
                    menu = true;
                    updateMenuDisplay();
                    break;
            }
        }
        handleMouseEvents(event);
    }

    SDL_Rect bgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    if (menuBackgroundTexture) {
        SDL_RenderCopy(renderer, menuBackgroundTexture, NULL, &bgRect);
    } else {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
    }

    SDL_Color titleColor = {255, 255, 0};
    SDL_Surface* titleSurface = TTF_RenderText_Solid(font2, "Settings", titleColor);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Rect titleRect = {SCREEN_WIDTH / 2 - titleSurface->w / 2, 100, titleSurface->w, titleSurface->h};
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

    for (int i = 0; i < 3; i++) {
         if (settingsOptionsTexture[i]) {
            SDL_RenderCopy(renderer, settingsOptionsTexture[i], NULL, &settingsOptionsRect[i]);
         }
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
     if (settingsOptionsTexture[selectedSettingsOption]) {
        SDL_Rect selectedRectHighlight = {
            settingsOptionsRect[selectedSettingsOption].x - 20,
            settingsOptionsRect[selectedSettingsOption].y - 5,
            settingsOptionsRect[selectedSettingsOption].w + 40,
            settingsOptionsRect[selectedSettingsOption].h + 10
        };
        SDL_RenderDrawRect(renderer, &selectedRectHighlight);
     }

    SDL_RenderPresent(renderer);
}
void Game::showGameModeMenu() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
            showingGameModeMenu = false;
        }
        else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                    selectedGameModeMenuOption = (selectedGameModeMenuOption - 1 + 3) % 3;
                    updateGameModeMenuDisplay();
                    break;
                case SDLK_DOWN:
                    selectedGameModeMenuOption = (selectedGameModeMenuOption + 1) % 3;
                    updateGameModeMenuDisplay();
                    break;
                case SDLK_RETURN:
                    handleGameModeMenuSelection();
                    break;
                 case SDLK_ESCAPE:
                    showingGameModeMenu = false;
                    menu = true;
                    updateMenuDisplay();
                    break;
            }
        }
        handleMouseEvents(event);
    }

    SDL_Rect bgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    if (menuBackgroundTexture) {
        SDL_RenderCopy(renderer, menuBackgroundTexture, NULL, &bgRect);
    } else {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
    }

    SDL_Color titleColor = {255, 255, 0};
    SDL_Surface* titleSurface = TTF_RenderText_Solid(font2, "Game Mode", titleColor);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Rect titleRect = {SCREEN_WIDTH / 2 - titleSurface->w / 2, 100, titleSurface->w, titleSurface->h};
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

    for (int i = 0; i < 3; i++) {
         if (gameModeMenuOptionsTexture[i]) {
            SDL_RenderCopy(renderer, gameModeMenuOptionsTexture[i], NULL, &gameModeMenuOptionsRect[i]);
         }
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
     if (gameModeMenuOptionsTexture[selectedGameModeMenuOption]) {
        SDL_Rect selectedRectHighlight = {
            gameModeMenuOptionsRect[selectedGameModeMenuOption].x - 20,
            gameModeMenuOptionsRect[selectedGameModeMenuOption].y - 5,
            gameModeMenuOptionsRect[selectedGameModeMenuOption].w + 40,
            gameModeMenuOptionsRect[selectedGameModeMenuOption].h + 10
        };
        SDL_RenderDrawRect(renderer, &selectedRectHighlight);
     }

    SDL_RenderPresent(renderer);
}

//////////////////
////Update t.t////
//////////////////
void Game::update() {
    if (isPause) return;
    //Update đạn
    if (player.active) player.updateBullets();
    if (gameMode == 2 && player2.active) player2.updateBullets();

    //Cử chỉ của enemy
    spawnEnemies();
    for (auto& enemy : enemies) {
        PlayerTank* target = nullptr;

        if (player.active && (!player2.active ||
            (abs(enemy.x - player.x) + abs(enemy.y - player.y) <= abs(enemy.x - player2.x) + abs(enemy.y - player2.y)))) {
            target = &player;
        } else if (player2.active) {
            target = &player2;
        }
        if (target) {
            enemy.moveTowardPlayer(target->x, target->y, walls, stones);
        }
        enemy.updateBullets();
        enemy.shoot(renderer);
    }

    //Va chạm tường
    for (auto& bullet : player.bullets) {
        for (auto& wall : walls) {
            if (wall.active && SDL_HasIntersection(&bullet.rect, &wall.rect)) {
                wall.active = false;
                bullet.active = false;
                wallExplosions.emplace_back(renderer, wall.rect.x, wall.rect.y);
                break;
            }
        }
    }
    for (auto& bullet : player2.bullets) {
        for (auto& wall : walls) {
            if (wall.active && SDL_HasIntersection(&bullet.rect, &wall.rect)) {
                wall.active = false;
                bullet.active = false;
                wallExplosions.emplace_back(renderer, wall.rect.x, wall.rect.y);
                break;
            }
        }
    }

    //kiểm tra va chạm đạn với địch
   for(auto& enemy: enemies){
     for(auto& bullet : player.bullets){
        for (auto& bullet2 : enemy.bullets){
            if (bullet.active && SDL_HasIntersection(&bullet.rect, &bullet2.rect)) {
            bullet.active = false;
            bullet2.active = false;
            break;

            }
        }
     }
   }
   for (auto& enemy: enemies){
     for (auto& bullet : player2.bullets){
        for (auto& bullet2 : enemy.bullets){
            if (bullet.active && SDL_HasIntersection(&bullet.rect, &bullet2.rect)) {
            bullet.active = false;
            bullet2.active = false;
            break;
            }
        }
     }
   }

   // Player bắn vào Boss
   if (level == 36 && bossActive && boss.active) {
    // Boss bắn và di chuyển đạn
    boss.shoot(renderer);
    boss.updateBullets();

    // Va chạm đạn Boss với Tường
    for (auto& wall : walls) {
        if (!wall.active) continue;
        // Đạn nhỏ
        for (auto& b : boss.bullets) {
            if (b.active && SDL_HasIntersection(&b.rect, &wall.rect)) {
                wall.active = false;
                b.active = false;
                wallExplosions.emplace_back(renderer, wall.rect.x, wall.rect.y);
            }
        }
        // Đạn to
        for (auto& b : boss.bulletsS) {
            if (b.active && SDL_HasIntersection(&b.rect, &wall.rect)) {
                wall.active = false;
                b.active = false;
                wallExplosions.emplace_back(renderer, wall.rect.x, wall.rect.y);
            }
        }
    }

    // Va chạm đạn Boss với Đá (đạn bị chặn, đá không vỡ)
    for (auto& stone : stones) {
        if (!stone.active) continue;
        for (auto& b : boss.bullets) {
            if (b.active && SDL_HasIntersection(&b.rect, &stone.rect)) {
                b.active = false;
            }
        }
        for (auto& b : boss.bulletsS) {
            if (b.active && SDL_HasIntersection(&b.rect, &stone.rect)) {
                b.active = false;
            }
        }
    }

    // Va chạm đạn Boss với Player
    for (auto& b : boss.bullets) {
        if (player.active && b.active && SDL_HasIntersection(&b.rect, &player.rect)) {
            player.hp--;
            b.active = false;
            if (player.hp <= 0) {
                Explosions.emplace_back(renderer, player.rect.x, player.rect.y);
                player.active = false;
                player.rect = {0,0,0,0};
            }
        }
    }
        for (auto& b : boss.bulletsS) {
            if (player.active && b.active && SDL_HasIntersection(&b.rect, &player.rect)) {
                player.hp--;
                b.active = false;
                if (player.hp <= 0) {
                    Explosions.emplace_back(renderer, player.rect.x, player.rect.y);
                    player.active = false;
                    player.rect = {0,0,0,0};
                }
            }
        }

    if (gameMode == 2) {
    for (auto& b : boss.bullets) {
        if (player2.active && b.active && SDL_HasIntersection(&b.rect, &player2.rect)) {
            player2.hp--;
            b.active = false;
            if (player2.hp <= 0) {
                Explosions.emplace_back(renderer, player2.rect.x, player2.rect.y);
                player2.active = false;
                player2.rect = {0,0,0,0};
            }
        }
    }
        for (auto& b : boss.bulletsS) {
            if (player2.active && b.active && SDL_HasIntersection(&b.rect, &player2.rect)) {
                player2.hp--;
                b.active = false;
                if (player2.hp <= 0) {
                    Explosions.emplace_back(renderer, player2.rect.x, player2.rect.y);
                    player2.active = false;
                    player2.rect = {0,0,0,0};
                }
            }
        }
    }

    for (auto& b : boss.bullets) {
        if (b.active && base.active && SDL_HasIntersection(&b.rect, &base.rect)) {
            base.hp--;
            b.active = false;
            if (base.hp <= 0) base.active = false;
        }
    }
    for (auto& b : boss.bulletsS) {
        if (b.active && base.active && SDL_HasIntersection(&b.rect, &base.rect)) {
            base.hp--;
            b.active = false;
            if (base.hp <= 0) base.active = false;
        }
    }

    // Player bắn vào boss (giữ nguyên như code cũ)
    for (auto& bullet : player.bullets) {
        if (SDL_HasIntersection(&bullet.rect, &boss.rect) && bullet.active) {
            bullet.active = false;
            boss.hp--;
            if (boss.hp <= 0) {
                boss.active = false;
                score += 1000;
                Explosions.emplace_back(renderer, boss.rect.x, boss.rect.y);
            }
        }
    }
    for (auto& bullet : player2.bullets) {
        if (SDL_HasIntersection(&bullet.rect, &boss.rect) && bullet.active) {
            bullet.active = false;
            boss.hp--;
            if (boss.hp <= 0) {
                boss.active = false;
                score += 1000;
                Explosions.emplace_back(renderer, boss.rect.x, boss.rect.y);
            }
        }
    }


    // Nếu có player2
    for (auto& bullet : player2.bullets) {
        if (SDL_HasIntersection(&bullet.rect, &boss.rect) && bullet.active) {
            bullet.active = false;
            boss.hp--;
            if (boss.hp <= 0) {
                boss.active = false;
                score += 1000;
                    Explosions.emplace_back(renderer, boss.rect.x, boss.rect.y);
                }
            }
        }
    }


   //Va chạm với đá
    for (auto& bullet : player.bullets) {
        for (auto& stone : stones) {
            if (stone.active && SDL_HasIntersection(&bullet.rect, &stone.rect)) {
                stone.active = true;
                bullet.active = false;
                break;
            }
        }
    }
    for (auto& bullet : player2.bullets) {
        for (auto& stone : stones) {
            if (stone.active && SDL_HasIntersection(&bullet.rect, &stone.rect)) {
                stone.active = true;
                bullet.active = false;
                break;
            }
        }
    }
    for (auto& enemy : enemies){
    for (auto& bullet : enemy.bullets) {
        for (auto& stone : stones) {
            if (stone.active && SDL_HasIntersection(&bullet.rect, &stone.rect)) {
                bullet.active = false;
                break;
            }
        }
    }
}

    //Player đi vào bụi cỏ
    for (auto& bush : bushs) {
    if (SDL_HasIntersection(&player.rect, &bush.rect)) {
        break;
    }
}

    //Player đi vào bùn
    for(auto& ice: ices){
        if(SDL_HasIntersection(&player.rect, &ice.rect)){
            tocdo1 = 2;
            break;
        }else tocdo1 = 4;
    }
    for(auto& ice: ices){
        if(SDL_HasIntersection(&player2.rect, &ice.rect)){
            tocdo2 = 2;
            break;
        }else tocdo2 = 4;
    }


    //Player bắn vào địch
    for (auto& bullet : player.bullets) {
        for (auto& enemy : enemies) {
            if (enemy.active && SDL_HasIntersection(&bullet.rect, &enemy.rect)) {
                enemy.hp--;
               // enemy.active = false;
                bullet.active = false;
                if (enemy.hp <= 0) {
                    enemy.active = false;
                    demslg1--;
                    Explosions.emplace_back(renderer, enemy.rect.x, enemy.rect.y);
                    score +=100;
                }
            }
        }
    }
    for (auto& bullet : player2.bullets) {
        for (auto& enemy : enemies) {
            if (enemy.active && SDL_HasIntersection(&bullet.rect, &enemy.rect)) {
               // enemy.active = false;
                bullet.active = false;
                if (enemy.hp <= 0) {
                    enemy.active = false;
                    demslg1--;
                    Explosions.emplace_back(renderer, enemy.rect.x, enemy.rect.y);
                    score +=100;
                }
            }
        }
    }

    //Va chạm đạn địch với tường và người chơi
    for (auto& enemy : enemies) {
        for (auto& bullet : enemy.bullets) {
            for (auto& wall : walls) {
                if (wall.active && SDL_HasIntersection(&bullet.rect, &wall.rect)) {
                    wallExplosions.emplace_back(renderer, wall.rect.x, wall.rect.y);
                    wall.active = false;
                    bullet.active = false;
                    break;
                }
            }
        }
    }
    for (auto& enemy : enemies) {
        for (auto& bullet : enemy.bullets) {
            if (SDL_HasIntersection(&bullet.rect, &player.rect)) {
                player.hp--;
                bullet.active = false;
                if(player.hp <= 0) {
                Explosions.emplace_back(renderer, player.rect.x, player.rect.y);
                player.active = false;
                player.rect = {0,0,0,0};
                }
            }else if(SDL_HasIntersection(&bullet.rect, &player2.rect)){
                player2.hp--;
                bullet.active = false;
                if(player2.hp <= 0) {
                Explosions.emplace_back(renderer, player2.rect.x, player2.rect.y);
                player2.active = false;
                player2.rect = {0,0,0,0};
                }
            }
        }

        if (gameMode == 1 && player.active == false){
            over = true;
        }else if (gameMode == 2 && player.active == false && player2.active == false){
            over = true;
        }
        else if (base.active == false){
            Explosions.emplace_back(renderer, base.rect.x, base.rect.y);
            over = true;
        }
    }

    //Va chạm với base
    for (auto& enemy : enemies ){
        for (auto& bullet: enemy.bullets){
            if(SDL_HasIntersection(&bullet.rect, &base.rect)){
                    base.hp--;
                    bullet.active = false;
                if(base.hp == 0){
                base.active = false;
                }
            }
        }
    }
    for (auto& bullet : player.bullets ){
            if(SDL_HasIntersection(&bullet.rect, &base.rect)){
                base.hp--;
                bullet.active = false;
                if(base.hp == 0){
                base.active = false;
              //  Explosions.emplace_back(renderer, base.rect.x, base.rect.y);
                }
            }
    }
    for (auto& bullet : player2.bullets ){
            if(SDL_HasIntersection(&bullet.rect, &base.rect)){
                base.hp--;
                bullet.active = false;
                if(base.hp == 0){
                base.active = false;
                }
            }
    }

    //Xóa kẻ địch bị đánh dấu enemies.active = false
    enemies.erase(
        std::remove_if(
            enemies.begin(),
            enemies.end(),
            [](EnemyTank& e) { return !e.active; }
        ),
        enemies.end()
    );

    //Xử lý vụ nổ
    wallExplosions.erase(
        std::remove_if(
            wallExplosions.begin(),
            wallExplosions.end(),
            [](const wallExplosion& explosion) { return explosion.isFinished(); }
        ),
        wallExplosions.end()
    );
    Explosions.erase(
    std::remove_if(
        Explosions.begin(),
        Explosions.end(),
        [](const Explosion& explosion) { return explosion.isFinished(); }
        ),
    Explosions.end()
    );

    if (level == 36 && bossActive && !boss.active) {
        showWinMessage();
        saveScore();
        SDL_Delay(1500);
        menu = true;
        over = false;
        bossActive = false;
        return;
    }

    //Kiểm tra điều kiện thắng
    if (enemiesSpawned >= 10 && enemies.empty()) {
        enemiesSpawned = 0;
        lastSpawnTime = 0;
        level++;
        // Nếu level trên 36 thì sẽ win luôn


    if (level == 36 && !bossActive) {
        bossActive = true;
        boss = BossTank(
            SCREEN_WIDTH / 2 - TILE_SIZE*5,
            SCREEN_HEIGHT / 2 - TILE_SIZE*3,
            renderer
        );
        boss.hp = isHardMode ? 30 : 20;
        // Cập nhật lại hiển thị level và máu boss (nếu có)
        updateLevelDisplay();
        updateBossHpDisplay();
        int oldHp = player.hp;
        player = PlayerTank(5 * TILE_SIZE, 13 * TILE_SIZE, renderer, "image/Tank.png",shootSound);
        player.hp = (oldHp > 0) ? oldHp : 1;
        if (gameMode == 2) {
            int oldHp2 = player2.hp;
            player2 = PlayerTank(9 * TILE_SIZE, 13 * TILE_SIZE, renderer, "image/Player2.png",shootSound);
            player2.hp = (oldHp2 > 0) ? oldHp2 : 1;
        }
        demslg1 = 0;
        tocdo1 = 4;
        tocdo2 = 4;
        player.bullets.clear();
        wallExplosions.clear();
        generateWalls();
        updateScoreDisplay();
        return;
    }

        //Nếu level dưới bằng 36 sẽ levelup
        if (level <=35 ) showLevelUpMessage();
        dangcap = std::to_string(level);

        int oldHp = player.hp;
        player = PlayerTank(5 * TILE_SIZE, 13 * TILE_SIZE, renderer, "image/Tank.png",shootSound);
        player.hp = (oldHp > 0) ? oldHp : 1;
        if (gameMode == 2){
            int oldHp2 = player2.hp;
            player2 = PlayerTank(9 * TILE_SIZE, 13 * TILE_SIZE, renderer, "image/Player2.png",shootSound);
            player2.hp = (oldHp2 > 0) ? oldHp2 : 1;
        }
        demslg1 = 10;
        tocdo1 = 4;
        tocdo2 = 4;
        updateLevelDisplay();
        updateScoreDisplay();
        player.bullets.clear();
        wallExplosions.clear();
        generateWalls();
        spawnEnemies();
        updateScoreDisplay();
    }
         if (gameMode == 1 && player.active == false){
            over = true;
        }else if (gameMode == 2 && player.active == false && player2.active == false){
            over = true;
        }
        else if (base.active == false){
            Explosions.emplace_back(renderer, base.rect.x, base.rect.y);
            over = true;
        }
};

////////////////////////////
////Hiển thị ra màn hình////
////////////////////////////
void Game::render() {
    if (isPause == true && menu == false ) {
        //Hiển thị thông báo Pause.
        // Render màn hình đen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Đen đặc
        SDL_RenderClear(renderer);

        // Render chữ "PAUSED"
        SDL_Color textColor = {255, 255, 0}; // Vàng
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, "PAUSED", textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = {SCREEN_WIDTH/2 - textSurface->w/2, SCREEN_HEIGHT/2 - 80, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        // Render hướng dẫn
        SDL_Color guideColor = {200, 255, 200};
        SDL_Surface* guideSurface = TTF_RenderUTF8_Solid(font, "Ấn ESC hoặc Z để tiếp tục, Q để về menu", guideColor);
        SDL_Texture* guideTexture = SDL_CreateTextureFromSurface(renderer, guideSurface);
        SDL_Rect guideRect = {SCREEN_WIDTH/2 - guideSurface->w/2, SCREEN_HEIGHT/2 + 20, guideSurface->w, guideSurface->h};
        SDL_RenderCopy(renderer, guideTexture, NULL, &guideRect);

        SDL_RenderPresent(renderer);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
        SDL_FreeSurface(guideSurface);
        SDL_DestroyTexture(guideTexture);
        return;

    }

    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int i = 1; i < MAP_HEIGHT-1; ++i) {
    for (int j = 1; j < MAP_WIDTH-6; ++j) {
            SDL_Rect tile = { j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE };
            SDL_RenderFillRect(renderer, &tile);
        }
    }
    // Thêm hàm render boss ở lv boss
    if (level == 36 && bossActive && boss.active) {
        boss.render(renderer);
    }

    for (auto& water: waters) water.render(renderer);
    for (auto& ice : ices) ice.render(renderer);
    for (auto& wall : walls) wall.render(renderer);
    for (auto& stone : stones) stone.render(renderer);

    if(player.active == true){
        player.render(renderer);
    }
    if(player2.active == true){
        player2.render(renderer);
    }
    if(base.active == true){
        base.render(renderer);
    }
    for (auto& enemy : enemies) enemy.render(renderer);
    for (auto& bush : bushs) bush.render(renderer);
    for (auto& explosion : wallExplosions) {
        explosion.render(renderer);
    }
    for (auto& tankex : Explosions){
        tankex.render(renderer);
    }
    SDL_RenderCopy(renderer, levelTextTexture, NULL, &levelTextRect);
    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
    SDL_RenderCopy(renderer, hpTexture,NULL, &hpRect);
    if (gameMode == 2 && hp2Texture) SDL_RenderCopy(renderer, hp2Texture, NULL, &hp2Rect);
    SDL_RenderCopy(renderer, baseHpTexture, NULL, &baseHpRect);
    SDL_RenderCopy(renderer, enemySpawmTexture, NULL, &enemySpawnRect);
    // Thêm mới;
    if (level == 36 && bossActive && boss.active) {
        if (bossHpLabelTexture) {
            SDL_RenderCopy(renderer, bossHpLabelTexture, NULL, &bossHpLabelRect);
        }
        if (bossHpBarTexture) {
            int hpBlockWidth = 20;
            int hpBlockSpacing = -1; // Khoảng cách nhỏ giữa các khối máu
            int startX = bossHpLabelRect.x + bossHpLabelRect.w + 10;
            int startY = bossHpLabelRect.y;

            for (int i = 0; i < boss.hp; ++i) {
                SDL_Rect hpRect = {startX + i * (hpBlockWidth + hpBlockSpacing), startY, hpBlockWidth, 25};
                SDL_RenderCopy(renderer, bossHpBarTexture, NULL, &hpRect);
            }
        }
    }
    SDL_RenderPresent(renderer);
}


void Game::run() {
    while (running) {
        if (menu) {
            showMenu();
        }
        else if (subMenu){
            showSubMenu();
        }
        else if (showingGameModeMenu) {
            showGameModeMenu();
        }
        else if (showingSettings) {
            showSettingsMenu();
        }
        else if (showingRanking) {
            showRanking();
        }
        else if (over) {
                SDL_Delay(1000);
                showGameOverMessage();
                over = false;
                saveScore(); // Lưu điểm
                SDL_Delay(200); // Đợi một chút để hiển thị thông điệp kết thúc
                menu = true; // Thoát game
        }
        else {
        handleEvents();
        update();
        updateScoreDisplay();
        updateHpDisplay();
        if (bossActive) updateBossHpDisplay();
        render();
        SDL_Delay(16);
        }
    }
}


////////////
////Save////
////////////
void Game::saveScore() {
    std::ofstream file("scores.txt", std::ios::app);
    if (file.is_open()) {
        file << score << std::endl;
        file.close();
        std::cout << "Score saved: " << score << std::endl;
    } else {
        std::cerr << "Unable to open scores.txt for writing" << std::endl;
    }
}
void Game::loadScores() {
    highScores.clear();
    std::ifstream file("scores.txt");

    if (file.is_open()) {
        int value;
        while (file >> value) {
            highScores.push_back(value);
        }
        file.close();

        // Sắp xếp điểm số từ cao đến thấp
        std::sort(highScores.begin(), highScores.end(), std::greater<int>());

        // Giữ lại tối đa 5 điểm cao nhất
        if (highScores.size() > 5) {
            highScores.resize(5);
        }
    } else {
        std::cout << "No scores file found, will create one when game ends" << std::endl;
    }
}

///////////////
////Ranking////
///////////////
void Game::showRanking() {
    // Xử lý sự kiện
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
            showingRanking = false;
        }
        else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_RETURN) {
                showingRanking = false;
                menu = true;
                updateMenuDisplay();
            }
        }
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Nền đen
    SDL_RenderClear(renderer);

    SDL_Color titleColor = {153, 51, 255}; // Màu tím hoàng kim sang trọng
    SDL_Surface* titleSurface = TTF_RenderText_Solid(font2, "RANKING", titleColor);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Rect titleRect = {SCREEN_WIDTH / 2 - titleSurface->w / 2, 30, titleSurface->w, titleSurface->h};
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

    updateRankingDisplay();

    SDL_Color textColor = {255, 255, 255}; // Màu trắng
    SDL_Surface* backSurface = TTF_RenderUTF8_Solid(font, "Ấn ESC hoặc Enter để quay trở lại menu", textColor);
    SDL_Texture* backTexture = SDL_CreateTextureFromSurface(renderer, backSurface);
    SDL_Rect backRect = {SCREEN_WIDTH / 2 - backSurface->w / 2, SCREEN_HEIGHT - 100, backSurface->w, backSurface->h};
    SDL_RenderCopy(renderer, backTexture, NULL, &backRect);
    SDL_FreeSurface(backSurface);
    SDL_DestroyTexture(backTexture);

    SDL_RenderPresent(renderer);
}
void Game::updateRankingDisplay() {

    std::vector<std::string> rankingLines;

    if (highScores.empty()) {
        rankingLines.push_back("No scores yet!");
    } else {
        for (size_t i = 0; i < highScores.size(); ++i) {
            rankingLines.push_back("#" + std::to_string(i + 1) + ": " + std::to_string(highScores[i]));
        }
    }
    if (rankingTexture) {
        SDL_DestroyTexture(rankingTexture);
        rankingTexture = nullptr;
    }

    SDL_Color textColor = {57, 255, 20};
    int startY = 200;
    int lineSpacing = 40;

    for (size_t i = 0; i < rankingLines.size(); ++i) {
        SDL_Surface* lineSurface = TTF_RenderText_Solid(font, rankingLines[i].c_str(), textColor);
        SDL_Texture* lineTexture = SDL_CreateTextureFromSurface(renderer, lineSurface);

        SDL_Rect lineRect = {
            SCREEN_WIDTH / 2 - lineSurface->w / 2, // Canh giữa theo chiều ngang
            startY + static_cast<int>(i) * lineSpacing,
            lineSurface->w,
            lineSurface->h
        };

        SDL_RenderCopy(renderer, lineTexture, NULL, &lineRect);
        // Giải phóng surface và texture sau khi render (quan trọng không lỗi game)
        SDL_FreeSurface(lineSurface);
        SDL_DestroyTexture(lineTexture);
    }
}


// Deconstructor~~
Game::~Game() {
    Mix_FreeMusic(backGroundMusic);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
