#include "Game.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <SDL_mixer.h>
#include <ctime>
Game::Game() {
    running = true;
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! Error: " << TTF_GetError() << std::endl;
        running = false;
    }
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        running = false;
        return;
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
    Mix_VolumeMusic(40);
    selectedOption = 0;
    selectedSubMenuOption = 0;
    menu = true;
    isPause = false;
    dangcap = std::to_string(level);
    srand(time(0));
    generateWalls();
   // base = Base(7 * TILE_SIZE, 13 * TILE_SIZE, renderer);
   // player = PlayerTank(5 * TILE_SIZE, 13 * TILE_SIZE,renderer, "image/Tank.png",shootSound);
   // player2 = PlayerTank(9 * TILE_SIZE,  13* TILE_SIZE,renderer, "image/Player2.png",shootSound);
   // player.hp = 3;
   // player2.hp = 3;
   // spawnEnemies();
    updateMenuDisplay();
   // updateSubMenuDisplay();
   // updateLevelDisplay();
   // updateScoreDisplay();
   // updateHpDisplay();
   // loadScores();
   // updateRankingDisplay();
}
void Game::resetGame() {
    over = false;
    menu = false;
    subMenu = false;
    isPause = false;
    score = 0;
    level = 1;
    enemiesSpawned = 0;
    lastSpawnTime = 0;

    player = PlayerTank(5 * TILE_SIZE, 13 * TILE_SIZE, renderer, "image/Tank.png", shootSound);
    player.hp = 3;
    player2 = PlayerTank(9 * TILE_SIZE, 13 * TILE_SIZE, renderer, "image/Player2.png", shootSound);
    player2.hp = 3;
    base = Base(7 * TILE_SIZE, 13 * TILE_SIZE, renderer);
    base.hp = 2;
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
    spawnEnemies();
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
    std::string timkiem = "map/" + dangcap + ".txt";
    std::cout<<timkiem;
    std::ifstream file(timkiem);
    for(int i= 1; i<=27; i++)
    {
        for (int j = 1; j<=26; j++){
            file >> Map[i][j];
        }
    }
    file.close();
    for (int i = 1; i<=27; i++){
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
    if (enemies.size() < 3 && enemiesSpawned < 10 && SDL_GetTicks() - lastSpawnTime >= 3000)
    {
        enemies.push_back(EnemyTank(SPAWN_POINT.x, SPAWN_POINT.y, renderer));
        enemiesSpawned++;
        lastSpawnTime = SDL_GetTicks();
        std::cout << "Spawned enemy at (" << SPAWN_POINT.x << ", " << SPAWN_POINT.y << ")\n";
    }
}


void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        // Xử lý sự kiện nhấn phím Z để tạm dừng (chỉ trigger khi nhấn, không giữ)
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_z) {
            isPause = !isPause;
        }
        else if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE){
            isPause = !isPause;
        }
        else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_1) {
            menu = 0;
        }
        else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_2 ){
            menu = 0;
        }
    }

    const Uint8* keystates = SDL_GetKeyboardState(nullptr); // Lấy trạng thái của tất cả các phím
    if(!isPause){
        if (player.active){
            if (keystates[SDL_SCANCODE_UP]) {
                player.move(0, -tocdo1, walls, stones);  // Di chuyển lên
            }
            if (keystates[SDL_SCANCODE_DOWN]) {
                player.move(0, tocdo1, walls, stones);   // Di chuyển xuống
            }
            if (keystates[SDL_SCANCODE_LEFT]) {
                player.move(-tocdo1, 0, walls, stones);  // Di chuyển sang trái
            }
            if (keystates[SDL_SCANCODE_RIGHT]) {
                player.move(tocdo1, 0, walls, stones);   // Di chuyển sang phải
            }
            if (keystates[SDL_SCANCODE_SPACE]) {
                player.shoot(renderer);  // Bắn đạn
            }
    }
    if(player2.active){
        if (keystates[SDL_SCANCODE_W]) {
            player2.move(0, -tocdo2, walls, stones);
        }
        if (keystates[SDL_SCANCODE_S]) {
            player2.move(0, tocdo2, walls, stones);
        }
        if (keystates[SDL_SCANCODE_A]) {
            player2.move(-tocdo2, 0, walls, stones);
        }
        if (keystates[SDL_SCANCODE_D]) {
            player2.move(tocdo2, 0, walls, stones);
        }
        if (keystates[SDL_SCANCODE_L]) {
            player2.shoot(renderer);
        }
        }
    }
}

void Game::updateLevelDisplay() {
    std::string strCapDo = "LEVEL: " + std::to_string(level);
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

}
void Game::showLevelUpMessage() {
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
}
void Game::showGameOverMessage() {
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

    // Tạo thông báo "Lưu điểm và quay trở lại menu chính"
    SDL_Color infoColor = {255, 255, 255}; // Màu trắng
    SDL_Surface* infoSurface = TTF_RenderUTF8_Solid(font, "Lưu điểm và quay trở lại menu chính", infoColor);
    SDL_Texture* infoTexture = SDL_CreateTextureFromSurface(renderer, infoSurface);

    // Vị trí hiển thị thông báo dưới cùng màn hình
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

    SDL_Delay(2000);
}
void Game::showWinMessage() {
    // Tạo thông báo "YOU WIN!"
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

    SDL_Delay(3000);
}
void Game::updateMenuDisplay() {
    SDL_Color titleColor = {255, 0, 0};
    SDL_Surface* titleSurface = TTF_RenderText_Solid(fontbrick, "TANKWAR", titleColor);
    titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    titleRect = {SCREEN_WIDTH / 2 - titleSurface->w / 2, 100, titleSurface->w, titleSurface->h};
    SDL_FreeSurface(titleSurface);

    // Tạo texture cho các tùy chọn menu
    for (int i = 0; i < 3; i++) {
        SDL_Color optionColor;
        if (i == selectedOption) {
            optionColor = {255, 255, 0}; // Màu vàng cho tùy chọn được chọn
        } else {
            optionColor = {255, 255, 255}; // Màu trắng cho các tùy chọn khác
        }

        SDL_Surface* optionSurface = TTF_RenderText_Solid(font, menuOptions[i], optionColor);
        optionsTexture[i] = SDL_CreateTextureFromSurface(renderer, optionSurface);
        optionsRect[i] = {SCREEN_WIDTH / 2 - optionSurface->w / 2, 250 + i * 70, optionSurface->w, optionSurface->h};
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
                    selectedOption = (selectedOption - 1 + 3) % 3;
                    updateMenuDisplay();
                    break;
                case SDLK_DOWN:
                    selectedOption = (selectedOption + 1) % 3;
                    updateMenuDisplay();
                    break;
                case SDLK_RETURN:
                    if (selectedOption == 0) {// Start Game
                        menu = false;
                        subMenu = true;
                        selectedSubMenuOption = 3;
                        updateSubMenuDisplay();
                    }
                    else if (selectedOption == 1) {// Hiển thị màn hình xếp hạng
                    showingRanking = true;
                    menu = false;
                    loadScores(); // Tải lại điểm số mới nhất
                    updateRankingDisplay();
                    }
                    else if (selectedOption == 2) {
                        // Exit
                        running = false;
                        menu = false;
                    }
                    break;

            }
        }
    }// Render menu
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);

    for (int i = 0; i < 3; i++) {
        SDL_RenderCopy(renderer, optionsTexture[i], NULL, &optionsRect[i]);
    }

    // Tạo hiệu ứng bằng hình chữ nhật xung quanh
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Màu vàng cho viền
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
                    if (selectedSubMenuOption == 0) { // 1 Player
                        resetGame();
                        gameMode = 1;
                        subMenu = false;
                        player2.active = false;
                        player2.rect = {0,0,0,0}; // Ẩn player 2 đi
                    }
                    else if (selectedSubMenuOption == 1) {
                        resetGame();
                        gameMode = 2;
                        subMenu = false;
                    }
                    else if (selectedSubMenuOption == 2) { // Back
                        subMenu = false; // Thoát subMenu
                        menu = true;     // Quay lại menu chính
                        updateMenuDisplay(); // Cập nhật lại menu chính
                    break;
                 case SDLK_ESCAPE: // Phím Escape cũng có thể dùng để Back
                    subMenu = false;
                    menu = true;
                    updateMenuDisplay();
                    break;
            }
        }
    }

    // Render menu con
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Màu đen cho nền
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);

    // Vẽ các tùy chọn menu con
    for (int i = 0; i < 3; i++) {
         if (subMenuOptionsTexture[i]) {
            SDL_RenderCopy(renderer, subMenuOptionsTexture[i], NULL, &subMenuOptionsRect[i]);
         }
    }

    // Vẽ hình chữ nhật hoặc mũi tên để chỉ lựa chọn hiện tại
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
}


void Game::update() {
    if (isPause) return;
    //Update đạn
    if (player.active) player.updateBullets();
    if (gameMode == 2 && player2.active) player2.updateBullets();

    //Cử chỉ của enemy
    spawnEnemies();
    for (auto& enemy : enemies) {
        enemy.moveTowardPlayer(player.x,player.y,walls,stones);
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
   for(auto& enemy: enemies){
     for(auto& bullet : player2.bullets){
        for (auto& bullet2 : enemy.bullets){
            if (bullet.active && SDL_HasIntersection(&bullet.rect, &bullet2.rect)) {
            bullet.active = false;
            bullet2.active = false;
            break;
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
    for(auto& enemy : enemies){
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

    //Player di vào băng
    for(auto& ice: ices){
        if(SDL_HasIntersection(&player.rect, &ice.rect)){
            tocdo1 = 4;
            break;
        }else tocdo1 = 4;
    }
    for(auto& ice: ices){
        if(SDL_HasIntersection(&player2.rect, &ice.rect)){
            tocdo2 = 4;
            break;
        }else tocdo2 = 4;
    }


    //Player bắn vào địch
    for (auto& bullet : player.bullets) {
        for (auto& enemy : enemies) {
            if (enemy.active && SDL_HasIntersection(&bullet.rect, &enemy.rect)) {
                enemy.active = false;
                bullet.active = false;
                score +=100;
            }
        }
    }
    for (auto& bullet : player2.bullets) {
        for (auto& enemy : enemies) {
            if (enemy.active && SDL_HasIntersection(&bullet.rect, &enemy.rect)) {
                enemy.active = false;
                bullet.active = false;
                score +=100;
            }
        }
    }


    for (auto& enemy : enemies) {
        for (auto& bullet : enemy.bullets) {
            for (auto& wall : walls) {
                if (wall.active && SDL_HasIntersection(&bullet.rect, &wall.rect)) {
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
                std::cout<<"player 1 bi ban hp con lai: "<<player.hp<<endl;
                bullet.active = false;
                if(player.hp <= 0) {player.active = false;
                player.rect = {0,0,0,0};
                }
            }else if(SDL_HasIntersection(&bullet.rect, &player2.rect)){
                player2.hp--;
                std::cout<<"player 1 bi ban hp con lai: "<<player2.hp<<endl;
                bullet.active = false;
                if(player2.hp <= 0) {player2.active = false;
                player2.rect = {0,0,0,0};
                }
            }
        }

        if (gameMode == 1 && player.active == false){
            over = true;
        }else if (gameMode == 2 && player.active == false && player2.active == false){
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
    wallExplosions.erase(
        std::remove_if(
            wallExplosions.begin(),
            wallExplosions.end(),
            [](const wallExplosion& explosion) { return explosion.isFinished(); }
        ),
        wallExplosions.end()
    );
    //Kiểm tra điều kiện thắng
    if (enemiesSpawned >= 10 && enemies.empty()) {
        enemiesSpawned = 0;
        lastSpawnTime = 0;
        level++;
        if (level > 35) {
            showWinMessage();
            saveScore();
            SDL_Delay(1500);
            menu = true;
            over = false;
        }
        if (level <=35 ) showLevelUpMessage();
        dangcap = std::to_string(level);
        int oldHp = player.hp;
        player = PlayerTank(5 * TILE_SIZE, 13 * TILE_SIZE, renderer, "image/Tank.png",shootSound);
        player.hp = 1;
        if(oldHp > 0) player.hp = oldHp;
        if (gameMode == 2){
            int oldHp2 = player2.hp;
            player2 = PlayerTank(9 * TILE_SIZE, 13 * TILE_SIZE, renderer, "image/Player2.png",shootSound);
            player2.hp = 1;
            if(oldHp2 > 0) player2.hp = oldHp2;
        }
        updateLevelDisplay();
        updateScoreDisplay();
        player.bullets.clear();
        wallExplosions.clear();
        generateWalls();
        spawnEnemies();
        updateScoreDisplay();
    }
};


void Game::render() {
    if (isPause == true && menu == false ) {
        //Hiển thị thông báo "PAUSED"
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, "PAUSED", {255, 255, 0});
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = {SCREEN_WIDTH/2 - 140 , SCREEN_HEIGHT/2 - 80, 200, 100};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_RenderPresent(renderer);
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
    SDL_RenderCopy(renderer, levelTextTexture, NULL, &levelTextRect);
    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
    SDL_RenderCopy(renderer, hpTexture,NULL, &hpRect);
    if (gameMode == 2 && hp2Texture) SDL_RenderCopy(renderer, hp2Texture, NULL, &hp2Rect);
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
        else if (showingRanking) {
            showRanking();
        }
        else if (over) {
                showGameOverMessage();
                over = false;
                saveScore(); // Lưu điểm
                SDL_Delay(100); // Đợi một chút để hiển thị thông điệp kết thúc (nếu cần)
                menu = true; // Thoát game
        }
        else {
        handleEvents();
        update();
        updateScoreDisplay();
        updateHpDisplay();
        render();
        SDL_Delay(16);
        }
    }
}
void Game::saveScore() {
    // Mở file để ghi (append mode)
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

        std::cout << "Loaded " << highScores.size() << " scores" << std::endl;
    } else {
        std::cout << "No scores file found, will create one when game ends" << std::endl;
    }
}
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
    // Xóa texture cũ (nếu có)
    if (rankingTexture) {
        SDL_DestroyTexture(rankingTexture);
        rankingTexture = nullptr;
    }

    SDL_Color textColor = {57, 255, 20}; // Màu xanh lá
    int startY = 200; // Vị trí Y bắt đầu render
    int lineSpacing = 40; // Khoảng cách giữa các dòng

    for (size_t i = 0; i < rankingLines.size(); ++i) {
        SDL_Surface* lineSurface = TTF_RenderText_Solid(font, rankingLines[i].c_str(), textColor);
        SDL_Texture* lineTexture = SDL_CreateTextureFromSurface(renderer, lineSurface);

        SDL_Rect lineRect = {
            SCREEN_WIDTH / 2 - lineSurface->w / 2, // Canh giữa theo chiều ngang
            startY + static_cast<int>(i) * lineSpacing, // Tính toán vị trí Y cho từng dòng
            lineSurface->w,
            lineSurface->h
        };

        SDL_RenderCopy(renderer, lineTexture, NULL, &lineRect);
        // Giải phóng surface và texture sau khi render (quan trọng không lỗi game)
        SDL_FreeSurface(lineSurface);
        SDL_DestroyTexture(lineTexture);
    }
}

Game::~Game() {
    Mix_FreeMusic(backGroundMusic);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
