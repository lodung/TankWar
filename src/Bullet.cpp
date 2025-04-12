#include "bullet.h"
#include "constants.h"
#include <SDL.h>
#include <SDL_image.h>
SDL_Texture *bulletTexture;
Bullet::Bullet(int startX, int startY, int dirX, int dirY, SDL_Renderer* renderer) {
    x = startX;
    y = startY;
    dx = dirX;
    dy = dirY;
    bulletTexture = IMG_LoadTexture(renderer,"image/bullet.jpg");
    active = true;
    rect = {x, y, 9, 9};
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
    x += dx;
    y += dy;
    rect.x = x;
    rect.y = y;
    if (x < TILE_SIZE || x > SCREEN_WIDTH - TILE_SIZE ||
        y < TILE_SIZE || y > SCREEN_HEIGHT - TILE_SIZE) {
        active = false;
    }
}

void Bullet::render(SDL_Renderer* renderer) {
    if (active) {
        //SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        //SDL_RenderFillRect(renderer, &rect);
        //SDL_RenderCopy(renderer, bulletTexture, NULL, &rect);
        SDL_RenderCopyEx(renderer, bulletTexture, NULL, &rect, angle, nullptr, SDL_FLIP_NONE);
    }
}
