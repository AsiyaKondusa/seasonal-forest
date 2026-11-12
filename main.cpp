#include <SDL2/SDL.h>
#include <cstdlib>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "mat4.h"

const int WIDTH = 800;
const int HEIGHT = 600;

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

// The current camera matrix. main() updates it every frame.
Mat4 gView = Mat4::identity();

// Moves a world point into camera view using the view matrix.
Vector3 toView(Vector3 v) {
    return transform(gView, v);
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
struct Mesh {
    std::vector<Vector3> verts;
    std::vector<std::pair<int,int>> edges;
};

Mesh loadOBJ(const std::string& filename) {
    Mesh mesh;
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string tag;
        ss >> tag;                       // first word: "v", "f", ...

        if (tag == "v") {                // a vertex: read x y z
            Vector3 v;
            ss >> v.x >> v.y >> v.z;
            mesh.verts.push_back(v);
        }
        else if (tag == "f") {           // a face: read its vertex numbers
            std::vector<int> idx;
            std::string token;
            while (ss >> token) {
                idx.push_back(std::stoi(token) - 1);   // OBJ counts from 1
            }
            // connect each vertex to the next, and the last back to the first
            for (size_t k = 0; k < idx.size(); k++) {
                mesh.edges.push_back({idx[k], idx[(k + 1) % idx.size()]});
            }
        }
    }
    return mesh;
}

void drawMesh(SDL_Renderer* r, const Mesh& m, const Mat4& model) {
    for (const auto& e : m.edges) {
        Vector3 a = transform(model, m.verts[e.first]);
        Vector3 b = transform(model, m.verts[e.second]);
        drawEdge3D(r, a, b);
    }
}

struct TreeInstance {
    Vector3 position;
    float scale;
    float yaw;      // rotation around Y, in radians
};

Mat4 modelMatrix(const TreeInstance& t) {
    return translation(t.position.x, t.position.y, t.position.z)
         * rotationY(t.yaw)
         * scaling(t.scale, t.scale, t.scale);
}

// Same as modelMatrix, but the canopy gets an extra scale s around its base (y = 1)
Mat4 canopyMatrix(const TreeInstance& t, float s) {
    return modelMatrix(t)
         * translation(0, 1, 0)
         * scaling(s, s, s)
         * translation(0, -1, 0);
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow(
        "Seasonal Forest",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT, 0);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    bool running = true;
    Mesh trunk  = loadOBJ("trunk.obj");
    Mesh canopy = loadOBJ("canopy.obj");
    if (trunk.verts.empty() || canopy.verts.empty()) {
        std::cout << "Could not load .obj files. Run from the forest folder.\n";
        return 1;
    }

        std::vector<TreeInstance> trees = {
        {{-3, 0, 3}, 1.0f, 0.2f},
        {{ 2, 0, 2}, 1.3f, 1.1f},
        {{-1, 0, 5}, 0.8f, 2.0f},
        {{ 3, 0, 6}, 1.1f, 0.7f},
        {{-4, 0, 7}, 1.2f, 1.7f},
        {{ 0, 0, 8}, 0.9f, 2.6f},
        {{ 4, 0, 9}, 1.0f, 0.4f},
    };

        float orbitAngle = 0.0f;
    
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
        }

          // 1. Move the camera
        orbitAngle += 0.01f;
        gView = translation(0, -3, 11) * rotationY(orbitAngle) * translation(0, 0, -5);

        // 2. Clear the screen to black
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        drawTerrain(renderer);

           // For now the canopy pulses between 60% and 140% size so you can see it work
        float canopyScale = 1.0f + 0.4f * sinf(orbitAngle * 2.0f);

        for (const auto& t : trees) {
            drawMesh(renderer, trunk,  modelMatrix(t));
            drawMesh(renderer, canopy, canopyMatrix(t, canopyScale));
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
