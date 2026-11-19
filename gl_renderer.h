#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <utility>
#include <vector>
#include "mat4.h"

struct Mesh {
    std::vector<Vector3> verts;
    std::vector<Vector3> normals;
    std::vector<std::vector<std::pair<int, int>>> faces; // (vertex, normal)
    std::vector<std::pair<int, int>> edges;              // legacy wireframe path
};

Mesh loadOBJ(const std::string& filename);

class GLRenderer {
public:
    GLRenderer(SDL_Window* window, int width, int height);
    ~GLRenderer();
    GLRenderer(const GLRenderer&) = delete;
    GLRenderer& operator=(const GLRenderer&) = delete;

    bool valid() const { return context != nullptr; }
    void beginFrame(const Mat4& view, float sunX, float sunY, float sunZ);
    void drawMesh(const Mesh& mesh, const Mat4& model, float red, float green, float blue);
    void drawPoint(const Vector3& position, float red, float green, float blue, float size = 5.0f);
    void endFrame();

private:
    SDL_Window* window;
    SDL_GLContext context;
};
