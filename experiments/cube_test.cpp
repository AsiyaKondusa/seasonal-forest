#include <SDL2/SDL.h>
#include <cstdlib>

const int WIDTH = 800;
const int HEIGHT = 600;
struct Vector3 { float x, y, z; };
struct Point2  { int x, y; };

Point2 project(const Vector3& v) {
    float focal = 400.0f;
    int sx = (int)( v.x * focal / v.z + WIDTH  / 2);
    int sy = (int)(-v.y * focal / v.z + HEIGHT / 2);
    return {sx, sy};
}

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

Vector3 cube[8] = {
    {-1,-1,-1}, { 1,-1,-1}, { 1, 1,-1}, {-1, 1,-1},   // back face (0-3)
    {-1,-1, 1}, { 1,-1, 1}, { 1, 1, 1}, {-1, 1, 1}    // front face (4-7)
};

int edges[12][2] = {
    {0,1},{1,2},{2,3},{3,0},    // back face
    {4,5},{5,6},{6,7},{7,4},    // front face
    {0,4},{1,5},{2,6},{3,7}     // connecting edges
};
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

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        for (int i = 0; i < 12; i++) {
            Vector3 a = cube[edges[i][0]];
            Vector3 b = cube[edges[i][1]];

            // Push the cube away from the camera so z is never 0 or negative
            a.x += 0.7f; a.y += 0.5f; a.z += 4.0f;
            b.x += 0.7f; b.y += 0.5f; b.z += 4.0f;

            Point2 pa = project(a);
            Point2 pb = project(b);
            drawLine(renderer, pa.x, pa.y, pb.x, pb.y);
        }
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
