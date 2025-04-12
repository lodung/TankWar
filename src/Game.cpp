
#include "Game.h"
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <SDL_mixer.h>

Game::Game() : enemyNumber(4) {
    running = true;
    if (SDL_Init(SDL_INIT_AUDIO) < 0 || Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    std::cerr << "SDL_mixer could not initialize! Error: " << Mix_GetError() << std::endl;
    running = false;
    return;
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
    Mix_VolumeMusic(40);
    generateWalls();
    player = PlayerTank(((MAP_WIDTH - 1) / 2) * TILE_SIZE, (MAP_HEIGHT - 2) * TILE_SIZE,renderer);
    spawnEnemies();
}

void Game::generateWalls() {
    walls.clear();
    stones.clear();
    char Map[26][26];
    std::ifstream file("map/1.txt");
    for(int i= 0; i<26; i++)
    {
        for (int j = 0 ; j<26; j++){
            file >> Map[i][j];
        }
    }
    file.close();
    for (int i = 0; i<26; i++){
        for(int j = 0; j<26; j++){
            if(Map[i][j] == '#'){
                Wall w = Wall((j+2)*TILE_SIZEm,(i+1)*TILE_SIZEm,renderer);
                walls.push_back(w);
            }
            if(Map[i][j] == '@'){
                Stone k = Stone((j+2)*TILE_SIZEm,(i+1)*TILE_SIZEm,renderer);
                stones.push_back(k);
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
        enemies.push_back(EnemyTank(ex, ey));
    }
}
void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        } else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_UP: player.move(0, -10, walls, stones); break;
                case SDLK_DOWN: player.move(0, 10, walls, stones); break;
                case SDLK_LEFT: player.move(-10, 0, walls, stones); break;
                case SDLK_RIGHT: player.move(10, 0, walls, stones); break;
                case SDLK_SPACE: player.shoot(renderer); break;
                case SDLK_ESCAPE: running = false;
            }
        }
    }
};
void Game::update() {
    player.updateBullets();

    for (auto& enemy : enemies) {
        enemy.moveTowardPlayer(player.x,player.y,walls,stones);
        enemy.updateBullets();
        if (rand() % 100 < 2) enemy.shoot(renderer);
    }
    for (auto& bullet : player.bullets) {
        for (auto& wall : walls) {
            if (wall.active && SDL_HasIntersection(&bullet.rect, &wall.rect)) {
                wall.active = false;
                bullet.active = false;
                break;
            }
        }
    }
    for (auto& bullet : player.bullets) {
        for (auto& stone : stones) {
            if (stone.active && SDL_HasIntersection(&bullet.rect, &stone.rect)) {
                stone.active = true;
                bullet.active = false;
                break;
            }
        }
    }
    // stones moi them vao
    for (auto& bullet : player.bullets) {
    for (auto& stone : stones) {
        if (stone.active && SDL_HasIntersection(&bullet.rect, &stone.rect)) {
            // Đá không bị phá hủy, chỉ hủy đạn
            bullet.active = false;
            break;
        }
    }
}
    for (auto& bullet : player.bullets) {
        for (auto& enemy : enemies) {
            if (enemy.active && SDL_HasIntersection(&bullet.rect, &enemy.rect)) {
                enemy.active = false;
                bullet.active = false;
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
                running = false;
                return;
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

    //Kiểm tra điều kiện thắng
    if (enemies.empty()) {
        std::cout<<"WIN";
        running = false;
    }
};
void Game::render() {
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int i = 1; i < MAP_HEIGHT-1; ++i) {
    for (int j = 1; j < MAP_WIDTH-3; ++j) {
        SDL_Rect tile = { j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE };
        SDL_RenderFillRect(renderer, &tile);
    }
    }

    for (auto& wall : walls) wall.render(renderer);
    for (auto& stone : stones) stone.render(renderer);
    player.render(renderer);
    for (auto& enemy : enemies) enemy.render(renderer);

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
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
