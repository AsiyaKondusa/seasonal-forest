#include <SDL2/SDL.h>
#include <cstdlib>

const int WIDTH = 800;
const int HEIGHT = 600;

void drawLine(SDL_Renderer* r, int x0, int y0, int x1, int y1) {
    int dx =  abs(x1 - x0);
    int dy = -abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;   // step direction in x
    int sy = (y0 < y1) ? 1 : -1;   // step direction in y
    int err = dx + dy;             // running error

    while (true) {
        SDL_RenderDrawPoint(r, x0, y0);
        if (x0 == x1 && y0 == y1) break;

        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }   // time to step in x
        if (e2 <= dx) { err += dx; y0 += sy; }   // time to step in y
    }
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow(
        "Seasonal Forest",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT, 0);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    bool running = true;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
        }

        // Black background
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // One white pixel in the middle
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        int cx = WIDTH / 2, cy = HEIGHT / 2;
        drawLine(renderer, cx, cy, cx + 200, cy);         // right
        drawLine(renderer, cx, cy, cx - 200, cy);         // left
        drawLine(renderer, cx, cy, cx, cy - 200);         // up
        drawLine(renderer, cx, cy, cx, cy + 200);         // down
        drawLine(renderer, cx, cy, cx + 150, cy - 100);   // shallow, up-right
        drawLine(renderer, cx, cy, cx - 100, cy + 180);   // steep, down-left
        drawLine(renderer, cx, cy, cx + 180, cy + 180);   // diagonal

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
