#include "EnemyTank.h"
#include <algorithm>
#include <SDL_image.h>
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

void EnemyTank::moveTowardPlayer(int playerX, int playerY, const vector<Wall>& walls,const vector<Stone>& stones) {
    if (--moveDelay > 0) return;

    moveDelay =100; // delay càng cao thì tank càng chậm

    int dx = 0, dy = 0;

    // Ưu tiên hướng có khoảng cách lớn hơn
    if (abs(playerX - x) > abs(playerY - y)) {
        dx = (playerX > x) ? 5 : -5;
    } else {
        dy = (playerY > y) ? 5 : -5;
    }

    // kiểm tra va chạm với tường
    int newX = x + dx;
    int newY = y + dy;
    SDL_Rect newRect = { newX, newY, TILE_SIZE, TILE_SIZE };

    for (const auto& wall : walls) {
        if (wall.active && SDL_HasIntersection(&newRect, &wall.rect)) {
            return; // va tường, không di chuyển
        }
    }
    for (const auto& stone : stones) {
        if (stone.active && SDL_HasIntersection(&newRect, &stone.rect)) {
            return; // va tường, không di chuyển
        }
    }

    // cập nhật vị trí
    x = newX;
    y = newY;
    rect.x = x;
    rect.y = y;
    dirX = dx;
    dirY = dy;

    if (dx < 0) dir = RIGHT;
    else if (dx > 0) dir = LEFT;
    else if (dy < 0) dir = DOWN;
    else if (dy > 0) dir = UP;
}


void EnemyTank::shoot(SDL_Renderer* renderer) {
    if (--shootDelay > 0) return;
    shootDelay = 5;
    bullets.push_back(Bullet(
        x + TILE_SIZE / 2 - 5,
        y + TILE_SIZE / 2 - 5,
        dirX, dirY, renderer
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
