class wallExplosion {
public:
    wallExplosion(SDL_Renderer* renderer, int x, int y)
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

    void render(SDL_Renderer* renderer) {
        Uint32 elapsed = SDL_GetTicks() - startTime;

        if (elapsed >= duration) return;

        currentFrame = elapsed / frameDelay;
        srcRect.y = currentFrame * frameHeight;

        SDL_RenderCopy(renderer, explosionTexture, &srcRect, &dstRect);
    }

    bool isFinished() const {
        return SDL_GetTicks() - startTime >= duration;
    }
private:
    SDL_Texture* explosionTexture = nullptr;

    SDL_Rect dstRect;
    SDL_Rect srcRect;

    Uint32 startTime;
    Uint32 duration;
    int frameCount;
    int currentFrame;
    int frameHeight;
    int frameDelay;
};
