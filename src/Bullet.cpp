#include "bullet.h"
#include "constants.h"
#include <SDL.h>
#include <iostream>
#include <SDL_image.h>

/////////////////////////////////
////// Bullet
////////////////////////////////
Bullet::Bullet(int startX, int startY, int dirX, int dirY, SDL_Renderer* renderer) {
    x = startX;
    y = startY;
    dx = dirX;
    dy = dirY;
    bulletTexture = IMG_LoadTexture(renderer,"image/bullet.jpg");
    active = true;
    rect = {x, y, 27, 27};
    calculateAngle();
}
void Bullet::calculateAngle() {
        if (dx > 0 ) {      // Phải
            angle = 90.0;
        } else if (dx < 0) { // Trái
            angle = 270.0;
        } else if (dy < 0) { // Lên
            angle = 0.0;
        } else if (dy > 0 ) {  // Xuống
            angle = 180.0;
        }
    };

void Bullet::move() {
    bulletSpeed = 7;
    if (dx > 0) dx = bulletSpeed;
    if (dx < 0) dx = -bulletSpeed;
    if (dy < 0) dy = -bulletSpeed;
    if (dy > 0) dy = bulletSpeed;
    x += dx;
    y += dy;
    rect.x = x;
    rect.y = y;
    if (x < TILE_SIZE || x > SCREEN_WIDTH - TILE_SIZE*6 || y < TILE_SIZE || y > SCREEN_HEIGHT - TILE_SIZE) {
        active = false;
    }
}

void Bullet::render(SDL_Renderer* renderer) {
    if (active) {
        SDL_RenderCopyEx(renderer, bulletTexture, NULL, &rect, angle, nullptr, SDL_FLIP_NONE);
    }
}

//////////////////////////////////////////////
//// BIG BULLET
/////////////////////////////////////////////
BigBullet::BigBullet(int startX, int startY, int dirX, int dirY, SDL_Renderer* renderer) {
    x = startX;
    y = startY;
    dx = dirX;
    dy = dirY;
    bigBulletTexture = IMG_LoadTexture(renderer, "image/nuclear.png");
    if (!bigBulletTexture) {
        std::cerr << "Failed to load nuclear texture: " << IMG_GetError() << std::endl;
    }
    active = true;
    rect = {x, y, 27, 27}; // To hơn đạn thường
    bigBulletSpeed = 4;
    calculateAngle();
}
void BigBullet::calculateAngle() {
    if (dx > 0 ) angle = 90.0;
    else if (dx < 0) angle = 270.0;
    else if (dy < 0) angle = 0.0;
    else if (dy > 0 ) angle = 180.0;
}
void BigBullet::move() {
    if (dx > 0) dx = bigBulletSpeed;
    if (dx < 0) dx = -bigBulletSpeed;
    if (dy < 0) dy = -bigBulletSpeed;
    if (dy > 0) dy = bigBulletSpeed;
    x += dx;
    y += dy;
    rect.x = x;
    rect.y = y;
    extern const int TILE_SIZE;
    extern const int SCREEN_WIDTH;
    extern const int SCREEN_HEIGHT;
    if (x < TILE_SIZE || x > SCREEN_WIDTH - TILE_SIZE*6 ||
        y < TILE_SIZE || y > SCREEN_HEIGHT - TILE_SIZE) {
        active = false;
    }
}
void BigBullet::render(SDL_Renderer* renderer) {
    if (active && bigBulletTexture) {
        SDL_RenderCopyEx(renderer, bigBulletTexture, NULL, &rect, angle, nullptr, SDL_FLIP_NONE);
    }
}
