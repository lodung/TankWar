
#include "Game.h"
#include <iostream>
#include <cstdlib>

Game::Game() : enemyNumber(4) {
    running = true;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        running = false;
    }

    window = SDL_CreateWindow("Battle City", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
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

    generateWalls();
    player = PlayerTank(((MAP_WIDTH - 1) / 2) * TILE_SIZE, (MAP_HEIGHT - 2) * TILE_SIZE);
    spawnEnemies();
}
Game::~Game() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
void Game::generateWalls() {
    walls.clear();
    for (int i = 1; i < MAP_HEIGHT - 1; ++i) {
        for (int j = 1; j < MAP_WIDTH - 1; ++j) {
            int wallX = j * TILE_SIZE;
            int wallY = i * TILE_SIZE;

            SDL_Rect playerZone = {
                ((MAP_WIDTH - 1) / 2) * TILE_SIZE - TILE_SIZE,
                (MAP_HEIGHT - 2) * TILE_SIZE - TILE_SIZE,
                TILE_SIZE * 3,
                TILE_SIZE * 3
            };
            SDL_Rect current = { wallX, wallY, TILE_SIZE, TILE_SIZE };
            if (SDL_HasIntersection(&playerZone, &current)) continue;

            if (rand() % 100 < WALL_CHANCE_PERCENT) {
                walls.push_back(Wall(wallX, wallY));
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
                case SDLK_UP: player.move(0, -5, walls); break;
                case SDLK_DOWN: player.move(0, 5, walls); break;
                case SDLK_LEFT: player.move(-5, 0, walls); break;
                case SDLK_RIGHT: player.move(5, 0, walls); break;
                case SDLK_SPACE: player.shoot(); break;
            }
        }
    }
}
void Game::update() {
    player.updateBullets();

    for (auto& enemy : enemies) {
        enemy.move(walls);
        enemy.updateBullets();
        if (rand() % 100 < 2) enemy.shoot();
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
    enemies.erase(
        std::remove_if(
            enemies.begin(),
            enemies.end(),
            [](EnemyTank& e) { return !e.active; }
        ),
        enemies.end()
    );

    if (enemies.empty()) {
        running = false;
    }
}
void Game::render() {
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int i = 1; i < MAP_HEIGHT - 1; ++i) {
        for (int j = 1; j < MAP_WIDTH - 1; ++j) {
            SDL_Rect tile = { j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE };
            SDL_RenderFillRect(renderer, &tile);
        }
    }

    for (auto& wall : walls) wall.render(renderer);
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
