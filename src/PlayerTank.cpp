#include "playertank.h"
#include "constants.h"
#include <SDL.h>
#include <algorithm>
PlayerTank::PlayerTank(int startX, int startY) {
    x = startX;
    y = startY;
    rect = {x, y, TILE_SIZE, TILE_SIZE};
    dirX = 0;
    dirY = -1;
}

// Constructor mặc định
PlayerTank::PlayerTank() {}

// Phương thức di chuyển
void PlayerTank::move(int dx, int dy, const std::vector<Wall>& walls) {
    int newX = x + dx;
    int newY = y + dy;
    this->dirX = dx;
    this->dirY = dy;

    SDL_Rect newRect = {newX, newY, TILE_SIZE, TILE_SIZE};
    for (const auto& wall : walls) {
        if (wall.active && SDL_HasIntersection(&newRect, &wall.rect)) {
            return;
        }
    }

    if (newX >= TILE_SIZE && newX <= SCREEN_WIDTH - TILE_SIZE * 2 &&
        newY >= TILE_SIZE && newY <= SCREEN_HEIGHT - TILE_SIZE * 2) {
        x = newX;
        y = newY;
        rect.x = x;
        rect.y = y;
    }
}

void PlayerTank::shoot() {
    bullets.push_back(Bullet(
        x + TILE_SIZE / 2 - 5,
        y + TILE_SIZE / 2 - 5,
        dirX, dirY
    ));
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
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &rect);
    for (auto& bullet : bullets) {
        bullet.render(renderer);
    }
}
