#include "playertank.h"
#include "constants.h"
#include <iostream>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <algorithm>
Uint32 lastShootTime = 0;
const Uint32 shootCooldown = 300;
void PlayerTank::calculateAngle() {
    if (dirX > 0 ) {      // Phải
        angle = 90.0;
        } else if (dirX < 0) { // Trái
          angle = 270.0;
        } else if (dirY < 0) { // Lên
          angle = 0.0;
        } else if (dirY > 0 ) {  // Xuống
          angle = 180.0;
        }
    };
PlayerTank::PlayerTank(int startX, int startY, SDL_Renderer* renderer, const std::string& texturePath,Mix_Chunk* sound) {
    x = startX;
    y = startY;
    rect = {x, y, TILE_SIZE, TILE_SIZE};
    active = true;
    tankTexture = IMG_LoadTexture(renderer,texturePath.c_str());
    dirX = 0;
    dirY = -1;
    shootSound = sound;
}

// Constructor mặc định
PlayerTank::PlayerTank() {}

// Phương thức di chuyển
void PlayerTank::move(int dx, int dy, const std::vector<Wall>& walls, const std::vector<Stone>& stones) {
    int newX = x + dx;
    int newY = y + dy;
    //this->
    dirX = dx;
    //this->
    dirY = dy;
    calculateAngle();
    SDL_Rect newRect = {newX, newY, TILE_SIZE, TILE_SIZE};
    for (const auto& wall : walls) {
        if (wall.active && SDL_HasIntersection(&newRect, &wall.rect)) {
            return; //CHỐNG VA TƯỜNG
        }
    }
    for (const auto& stone : stones) {
        if (stone.active && SDL_HasIntersection(&newRect, &stone.rect)) {
            return; //CHỐNG VA TƯỜNG
        }
    }

    if (newX >= TILE_SIZE && newX <= SCREEN_WIDTH - TILE_SIZE * 6 - TILE_SIZEm*2  &&
        newY >= TILE_SIZE && newY <= SCREEN_HEIGHT - TILE_SIZE * 2) {
        x = newX;
        y = newY;
        rect.x = x;
        rect.y = y;
    }
}

void PlayerTank::shoot(SDL_Renderer* renderer) {
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastShootTime >= shootCooldown) {
    bullets.push_back(Bullet(
        x + TILE_SIZE / 2 - 5,
        y + TILE_SIZE / 2 - 5,
        dirX, dirY,renderer
    ));
    if (shootSound) {
        Mix_PlayChannel(-1, shootSound, 0);
    }
        lastShootTime = currentTime;
    }
}
void PlayerTank::updateBullets() {
    bullets.erase(
        std::remove_if(
            bullets.begin(),
            bullets.end(),
            [](Bullet& b) { return !b.active; }
        ),
        bullets.end()
    );
    for (auto& bullet : bullets) {
        bullet.move();
    }
}
void PlayerTank::render(SDL_Renderer* renderer) {
    if (active){
    if (tankTexture) {
        SDL_RenderCopyEx(renderer, tankTexture, NULL, &rect, angle, nullptr, SDL_FLIP_NONE);
    }
    //SDL_RenderCopy(renderer, tankTexture, NULL, &rect);
    for (auto& bullet : bullets) {
        bullet.render(renderer);
        }
    }
}
