#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <algorithm>
#include "Wall.h" 8
#include "Constants.h"
#include "PlayerTank.h"
#include "Bullet.h"
#include "EnemyTank.h"
#include "Game.h"
using namespace std;

int main(int argc, char* argv[]) {
    Game game;
    if (game.running) {
        game.run();
    }
    return 0;
}
