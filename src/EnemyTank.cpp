#include "EnemyTank.h"
#include <algorithm>
#include <iostream>
#include <SDL_image.h>
#include <cstdlib>

EnemyTank::EnemyTank(int startX, int startY, SDL_Renderer *renderer, int health) {
    moveDelay = 5;
    x = startX;
    y = startY;
    hp = health;
    enemyTexture = IMG_LoadTexture(renderer, "image/enemy1.png");
    rect = {x, y, TILE_SIZE, TILE_SIZE};
    dirX = 0;
    dirY = 1;
    active = true;
    calculateAngle();
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
        int randomIndex = rand() % 4;
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


////////////////////////
////////BOSS //////////
//////////////////////

BossTank::BossTank() : x(0), y(0), hp(0), bossTexture(nullptr), active(false), lastShootTime(0), shootCooldown(1000) {
    rect = {0, 0, 0, 0};
}

BossTank::BossTank(int startX, int startY, SDL_Renderer* renderer) {
    x = startX;
    y = startY;
    hp = 20;
    active = true;
    lastShootTime = 0;
    shootCooldown = 1200;
    lastRotateTime = 0;
    angle = 0.0;
    bossTexture = IMG_LoadTexture(renderer, "image/boss.png");
    rect = {x, y, TILE_SIZE*5, TILE_SIZE*5};
}

void BossTank::shoot(SDL_Renderer* renderer) {
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastShootTime < shootCooldown) return;
    const int dirs[4][2] = { {0, -10}, {0, 10}, {-10, 0}, {10, 0} };
    int cx = x + rect.w / 2;
    int cy = y + rect.h / 2;
    int offset = 32;
    for (int i = 0; i < 4; ++i) {
        int vx = dirs[i][0];
        int vy = dirs[i][1];
        // Đạn to ở giữa
        bulletsS.emplace_back(cx - 27, cy - 10, vx, vy, renderer);
        // Đạn nhỏ lệch hai bên
        if (vy != 0) {
            bullets.emplace_back(cx - 5 - offset, cy - 5, vx, vy, renderer);
            bullets.emplace_back(cx - 5 + offset, cy - 5, vx, vy, renderer);
        } else {
            bullets.emplace_back(cx - 5, cy - 5 - offset, vx, vy, renderer);
            bullets.emplace_back(cx - 5, cy - 5 + offset, vx, vy, renderer);
        }
    }
    lastShootTime = currentTime;
}

void BossTank::update() {
    Uint32 currentTime = SDL_GetTicks();

    if (currentTime - lastRotateTime > 200) { // Cứ 200ms xoay 1 lần
        int randomAngle = (rand() % 4) * 90;
        angle = randomAngle;
        lastRotateTime = currentTime;
    }
    for (auto& b : bullets) b.move();
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
            [](const Bullet& b){ return !b.active; }),
        bullets.end()
    );
    for (auto& b : bulletsS) b.move();
    bulletsS.erase(
        std::remove_if(bulletsS.begin(), bulletsS.end(),
            [](const BigBullet& b){ return !b.active; }),
        bulletsS.end()
    );
}

void BossTank::render(SDL_Renderer* renderer) {
    if (active) {
        SDL_RenderCopyEx(renderer, bossTexture, NULL, &rect, angle, nullptr, SDL_FLIP_NONE);
    }
    // Hiển thị hp:
    for (auto& bullet : bullets) bullet.render(renderer);
    for (auto& bullet : bulletsS) bullet.render(renderer);
}
