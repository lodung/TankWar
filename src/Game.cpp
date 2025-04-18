
#include "Game.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <SDL_mixer.h>
#include <ctime>
Game::Game() : enemyNumber(1) {
    running = true;

    if (SDL_Init(SDL_INIT_AUDIO) < 0 || Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    std::cerr << "SDL_mixer could not initialize! Error: " << Mix_GetError() << std::endl;
    running = false;
    return;
    }
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! Error: " << TTF_GetError() << std::endl;
        running = false;
    }
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        running = false;
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
    font = TTF_OpenFont("ARIAL.ttf", 150);
    if (!font) {
        std::cerr << "Failed to load font! Error: " << TTF_GetError() << std::endl;
        running = false;
    }
    Mix_VolumeMusic(40);
    level = 17;
    score = 0;
    tocdo1 = 2; tocdo2 = 2; menu = true;
    isPause = 0;
    dangcap = std::to_string(level);
    srand(time(0));
    generateWalls();
    base = Base (9*TILE_SIZEm, 13*TILE_SIZEm, renderer);
    player = PlayerTank(5 * TILE_SIZE, 13 * TILE_SIZE,renderer, "image/Tank.png");
    player2 = PlayerTank(9 * TILE_SIZE,  13* TILE_SIZE,renderer, "image/Player2.png");
    spawnEnemies();

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
    enemies.clear();
    for (int i = 0; i < enemyNumber; ++i) {
        int ex, ey;
        bool validPosition = false;
        while (!validPosition) {
            ex = (rand() % (MAP_WIDTH - 2) + 1) * TILE_SIZE;
            ey = (rand() % (MAP_HEIGHT - 2) + 1) * TILE_SIZE;
            validPosition = true;
            for (const auto& wall : walls) {
                if (wall.active && wall.x == ex && wall.y == ey) {
                    validPosition = false;
                    break;
                }
            }
        }
        enemies.push_back(EnemyTank(ex, ey,renderer));
    }
}
       /* void Game::handleEvents() {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = false;
                }
                if (event.key.keysym.sym == SDLK_z)
                    {
                        isPause = !isPause;
                    }
                if (!isPause){

                if (event.type == SDL_KEYDOWN) {
                    switch (event.key.keysym.sym) {
                        case SDLK_UP: player.move(0, -tocdo1, walls, stones); break;
                        case SDLK_DOWN: player.move(0, tocdo1, walls, stones); break;
                        case SDLK_LEFT: player.move(-tocdo1, 0, walls, stones); break;
                        case SDLK_RIGHT: player.move(tocdo1, 0, walls, stones); break;
                        case SDLK_SPACE: player.shoot(renderer); break;
                        case SDLK_ESCAPE: running = false; break;
                        case SDLK_w: player2.move(0, -tocdo2, walls, stones); break;
                        case SDLK_s: player2.move(0, tocdo2, walls, stones); break;
                        case SDLK_a: player2.move(-tocdo2, 0, walls, stones); break;
                        case SDLK_d: player2.move(tocdo2, 0, walls, stones); break;
                        case SDLK_l: player2.shoot(renderer); break;
                        }
                    }
                }
            }
};*/
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
            running = false;
        }
        else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_1) {
            menu = 0;
        }
        else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_2 ){
            menu = 0;
        }
    }

    const Uint8* keystates = SDL_GetKeyboardState(nullptr); // Lấy trạng thái của tất cả các phím

    // Di chuyển Player 1

    if(!isPause){
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

    // Di chuyển Player 2
    if (keystates[SDL_SCANCODE_W]) {
        player2.move(0, -tocdo2, walls, stones); // Di chuyển lên
    }
    if (keystates[SDL_SCANCODE_S]) {
        player2.move(0, tocdo2, walls, stones);  // Di chuyển xuống
    }
    if (keystates[SDL_SCANCODE_A]) {
        player2.move(-tocdo2, 0, walls, stones); // Di chuyển sang trái
    }
    if (keystates[SDL_SCANCODE_D]) {
        player2.move(tocdo2, 0, walls, stones);  // Di chuyển sang phải
    }
    if (keystates[SDL_SCANCODE_L]) {
        player2.shoot(renderer);  // Bắn đạn
    }
    }
}




void Game::update() {
    if (isPause) return;
    //Update đạn
    player.updateBullets();
    player2.updateBullets();

    //Cử chỉ của enemy
    for (auto& enemy : enemies) {
        enemy.moveTowardPlayer(player.x,player.y,walls,stones);
        enemy.updateBullets();
        if (rand() % 100 < 2) enemy.shoot(renderer);
    }

    //Va chạm tường
    for (auto& bullet : player.bullets) {
        for (auto& wall : walls) {
            if (wall.active && SDL_HasIntersection(&bullet.rect, &wall.rect)) {
                wall.active = false;
                bullet.active = false;
                break;
            }
        }
    }
    for (auto& bullet : player2.bullets) {
        for (auto& wall : walls) {
            if (wall.active && SDL_HasIntersection(&bullet.rect, &wall.rect)) {
                wall.active = false;
                bullet.active = false;
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
            bullet2.active =false;
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
            bullet2.active =false;
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

        if (player2.hp == 0 && player.hp == 0){

            running = false;
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

    //Kiểm tra điều kiện thắng
    if (enemies.empty()) {
        std::cout << "WIN" << std::endl;
        level++;
        if (level > 35) {
            std::cout<<"diem so: "<<score;
            SDL_Delay(1500);
            running = false;
        }
        dangcap = std::to_string(level);
        int oldHp = player.hp;
        player = PlayerTank(5 * TILE_SIZE, 13 * TILE_SIZE, renderer, "image/Tank.png");
        if(oldHp > 0) player.hp = oldHp;
        int oldHp2 = player2.hp;
        player2 = PlayerTank(9 * TILE_SIZE, 13 * TILE_SIZE, renderer, "image/Player2.png");
        if(oldHp2 > 0) player2.hp = oldHp2;

        player.bullets.clear();
        generateWalls();
        spawnEnemies();
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
    for (int j = 1; j < MAP_WIDTH-4; ++j) {
        SDL_Rect tile = { j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE };
        SDL_RenderFillRect(renderer, &tile);
    }
    }
    //for (auto& bush : bushs) bush.render(renderer);
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
    for (auto& enemy : enemies) enemy.render(renderer);
    for (auto& bush : bushs) bush.render(renderer);
    if(base.active == true) base.render(renderer);
    SDL_RenderPresent(renderer);
}


void Game::run() {
    while (running) {
        handleEvents();
        update();
        render();
        SDL_Delay(16);
    }
}


Game::~Game() {
    Mix_FreeMusic(backGroundMusic);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
