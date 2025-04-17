class Explosion {
public:
    SDL_Rect rect; // vị trí phát nổ
    SDL_Texture* texture;
    int frame = 0;
    int totalFrames = 6;
    int frameDelay = 5; // số frame logic chờ trước khi chuyển ảnh
    int delayCounter = 0;
    bool finished = false;

    Explosion(SDL_Renderer* renderer, int x, int y, const std::string& path) {
        texture = IMG_LoadTexture(renderer, path.c_str());
        rect = {x, y, 64, 64};
    }

    void update() {
        if (finished) return;
        delayCounter++;
        if (delayCounter >= frameDelay) {
            delayCounter = 0;
            frame++;
            if (frame >= totalFrames) {
                finished = true;
            }
        }
    }

    void render(SDL_Renderer* renderer) {
        if (finished) return;
        SDL_Rect src = {frame * 64, 0, 64, 64}; // chọn frame từ sprite
        SDL_RenderCopy(renderer, texture, &src, &rect);
    }

    ~Explosion() {
        SDL_DestroyTexture(texture);
    }
};
