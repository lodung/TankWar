#include "EnemyTank.h"
#include <algorithm>

EnemyTank::EnemyTank(int startX, int startY) {
    moveDelay = 15;
    shootDelay = 5;
    x = startX;
    y = startY;
    rect = {x, y, TILE_SIZE, TILE_SIZE};
    dirX = 0;
    dirY = 1;
    active = true;
}

void EnemyTank::move(const std::vector<Wall>& walls) {
    if (--moveDelay > 0) return;
    moveDelay = 15;
    int r = rand() % 4;
    if (r == 0) { dirX = 0; dirY = -5; }
    else if (r == 1) { dirX = 0; dirY = 5; }
    else if (r == 2) { dirY = 0; dirX = -5; }
    else if (r == 3) { dirY = 0; dirX = 5; }
}

void EnemyTank::shoot() {
    if (--shootDelay > 0) return;
    shootDelay = 5;
    bullets.push_back(Bullet(
        x + TILE_SIZE / 2 - 5,
        y + TILE_SIZE / 2 - 5,
        dirX, dirY
    ));
}

void EnemyTank::updateBullets() {
    for (auto &bullet : bullets) bullet.move();
    bullets.erase(
        std::remove_if(
            bullets.begin(),
            bullets.end(),
            [](Bullet &b) { return !b.active; }
        ),
        bullets.end()
    );
}

void EnemyTank::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &rect);
    for (auto &bullet : bullets) bullet.render(renderer);
}
