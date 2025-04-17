#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <string>
#include <algorithm>
#include "src/Game.h"

int main(int argc, char* argv[]) {
    //SDL_Texture* backgroundTexture = IMG_LoadTexture(renderer, "image/background.png");

    Game game;
    if (game.running) {
        game.run();
    }
    return 0;
}
