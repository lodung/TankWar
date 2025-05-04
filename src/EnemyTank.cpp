#include "EnemyTank.h"
#include <algorithm>
#include <iostream>
#include <SDL_image.h>

EnemyTank::EnemyTank(int startX, int startY, SDL_Renderer *renderer) {
    moveDelay = 5;
    x = startX;
    y = startY;
    enemyTexture = IMG_LoadTexture(renderer, "image/enemy1.png");
    rect = {x, y, TILE_SIZE, TILE_SIZE};
    dirX = 0;
    dirY = 1;
    active = true;
}

void EnemyTank::calculateAngle() {
    if (dirX > 0) {      // Phải
        angle = 90.0;
    } else if (dirX < 0) { // Trái
        angle = 270.0;
    } else if (dirY < 0) { // Lên
        angle = 0.0;
    } else if (dirY > 0) {  // Xuống
        angle = 180.0;
    }
};

void EnemyTank::moveTowardPlayer(int playerX, int playerY, const vector<Wall>& walls, const vector<Stone>& stones) {
    if (--moveDelay > 0) return;

    moveDelay = 5;

    Uint32 currentTime = SDL_GetTicks(); //biến để kiểm soát thời gian
    static Uint32 lastChangeTime = 0;    //Lưu thời điểm lần cuối thay đổi não
    static bool chasingPlayer = true;

    // (+) Chuyển đổi giữa chế độ đuổi theo và di chuyển ngẫu nhiên sau mỗi 3 giây
    if (currentTime - lastChangeTime > 3000) {
        lastChangeTime = currentTime;
        chasingPlayer = !chasingPlayer;
    }

    if (chasingPlayer) {

        int dxPrimary = 0, dyPrimary = 0;
        int dxSecondary = 0, dySecondary = 0;

        if (abs(playerX - x) > abs(playerY - y)) {
            dxPrimary = (playerX > x) ? 5 : -5;
            dySecondary = (playerY > y) ? 5 : (playerY < y ? -5 : 0);
        } else {
            dyPrimary = (playerY > y) ? 5 : -5;
            dxSecondary = (playerX > x) ? 5 : (playerX < x ? -5 : 0);
        }

        if (canMove(x + dxPrimary, y + dyPrimary, walls, stones)) {
            applyMove(dxPrimary, dyPrimary);
            return;
        }

        if (canMove(x + dxSecondary, y + dySecondary, walls, stones)) {
            applyMove(dxSecondary, dySecondary);
            return;
        }
    } else {
        const int directions[4][2] = {{10, 0}, {-10, 0}, {0, 10}, {0, -10}};
        int randomIndex = rand() % 4; // Chọn hướng ngẫu nhiên

        if (canMove(x + directions[randomIndex][0], y + directions[randomIndex][1], walls, stones)) {
            applyMove(directions[randomIndex][0], directions[randomIndex][1]);
        }
    }
}

bool EnemyTank::canMove(int newX, int newY, const vector<Wall>& walls, const vector<Stone>& stones) {
    if (newX < TILE_SIZE || newX > SCREEN_WIDTH - TILE_SIZE * 5 - TILE_SIZE * 2 ||
        newY < TILE_SIZE || newY > SCREEN_HEIGHT - TILE_SIZE * 2) {
        return false;
    }
    SDL_Rect newRect = {newX, newY, TILE_SIZE, TILE_SIZE};
    for (const auto& wall : walls) {
        if (wall.active && SDL_HasIntersection(&newRect, &wall.rect)) return false;
    }
    for (const auto& stone : stones) {
        if (stone.active && SDL_HasIntersection(&newRect, &stone.rect)) return false;
    }
    return true;
}

void EnemyTank::applyMove(int dx, int dy) {
    x += dx;
    y += dy;
    rect.x = x;
    rect.y = y;
    dirX = dx;
    dirY = dy;

    if (dx < 0) dir = RIGHT;
    else if (dx > 0) dir = LEFT;
    else if (dy < 0) dir = DOWN;
    else if (dy > 0) dir = UP;
    calculateAngle();
}

void EnemyTank::shoot(SDL_Renderer* renderer) {
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastShootTime >= shootCooldown) {
        bullets.push_back(Bullet(
            x + TILE_SIZE / 2 - 5,
            y + TILE_SIZE / 2 - 5,
            dirX, dirY, renderer
        ));
        lastShootTime = currentTime;
    }
}

void EnemyTank::updateBullets() {
    for (auto& bullet : bullets) bullet.move();
    bullets.erase(
        std::remove_if(
            bullets.begin(),
            bullets.end(),
            [](Bullet& b) { return !b.active; }
        ),
        bullets.end()
    );
}

void EnemyTank::render(SDL_Renderer* renderer) {
    SDL_RenderCopyEx(renderer, enemyTexture, NULL, &rect, angle, nullptr, SDL_FLIP_NONE);
    for (auto& bullet : bullets) bullet.render(renderer);
}
