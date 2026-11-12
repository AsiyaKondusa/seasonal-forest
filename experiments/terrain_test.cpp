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

// Moves a world point into camera view: camera is 2 units above the ground,
// and the world is pushed 6 units away from the camera.
Vector3 toView(Vector3 v) {
    v.y -= 2.0f;
    v.z += 6.0f;
    return v;
}

// Draw one 3D edge: move into view, skip if behind the camera, project, draw.
void drawEdge3D(SDL_Renderer* r, Vector3 a, Vector3 b) {
    a = toView(a);
    b = toView(b);
    if (a.z < 0.1f || b.z < 0.1f) return;   // never divide by z near 0
    Point2 pa = project(a);
    Point2 pb = project(b);
    drawLine(r, pa.x, pa.y, pb.x, pb.y);
}

void drawTerrain(SDL_Renderer* r) {
    const int N = 10;             // 10 x 10 squares
    const float spacing = 1.0f;   // distance between points

    for (int i = 0; i <= N; i++) {          // i walks along X
        for (int j = 0; j <= N; j++) {      // j walks along Z (depth)
            Vector3 p = { (i - N / 2) * spacing, 0.0f, j * spacing };

            if (i < N) {   // line to the neighbor on the right
                Vector3 right = { (i + 1 - N / 2) * spacing, 0.0f, j * spacing };
                drawEdge3D(r, p, right);
            }
            if (j < N) {   // line to the neighbor behind
                Vector3 back = { (i - N / 2) * spacing, 0.0f, (j + 1) * spacing };
                drawEdge3D(r, p, back);
            }
        }
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

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        drawTerrain(renderer);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
