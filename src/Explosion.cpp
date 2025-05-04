#include "Explosion.h"
wallExplosion :: wallExplosion(SDL_Renderer* renderer, int x, int y)
        : startTime(SDL_GetTicks()), duration(500), frameCount(5), currentFrame(0), frameHeight(30)
    {
        explosionTexture = IMG_LoadTexture(renderer, "image/wallExplosion.png");
           dstRect = { x, y,  TILE_SIZEm,  TILE_SIZEm };
        srcRect = { 0, 0,  30,  30 };

        frameDelay = duration / frameCount;
    }

    ~wallExplosion() {
        if (explosionTexture) SDL_DestroyTexture(explosionTexture);
    }

wallExplosion::   void render(SDL_Renderer* renderer) {
        Uint32 elapsed = SDL_GetTicks() - startTime;

        if (elapsed >= duration) return;

        currentFrame = elapsed / frameDelay;
        srcRect.y = currentFrame * frameHeight;

        SDL_RenderCopy(renderer, explosionTexture, &srcRect, &dstRect);
    }

 wallExplosion::   bool isFinished() const {
        return SDL_GetTicks() - startTime >= duration;
    }
