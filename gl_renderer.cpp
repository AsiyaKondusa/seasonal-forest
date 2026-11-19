#include "gl_renderer.h"

#include <SDL2/SDL_opengl.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>

namespace {
void matrixData(const Mat4& matrix, GLfloat data[16]) {
    // Mat4 uses row/column indexing and column vectors; OpenGL consumes column-major data.
    for (int row = 0; row < 4; ++row)
        for (int col = 0; col < 4; ++col)
            data[col * 4 + row] = matrix.m[row][col];
}

void loadMatrix(const Mat4& matrix) {
    GLfloat data[16];
    matrixData(matrix, data);
    glLoadMatrixf(data);
}

bool parseIndex(const std::string& field, int count, int& result) {
    if (field.empty()) return false;
    const int raw = std::stoi(field);
    result = raw > 0 ? raw - 1 : count + raw; // OBJ also permits negative relative indices
    return result >= 0 && result < count;
}
}

Mesh loadOBJ(const std::string& filename) {
    Mesh mesh;
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream stream(line);
        std::string tag;
        stream >> tag;
        if (tag == "v") {
            Vector3 v{};
            if (stream >> v.x >> v.y >> v.z) mesh.verts.push_back(v);
        } else if (tag == "vn") {
            Vector3 n{};
            if (stream >> n.x >> n.y >> n.z) mesh.normals.push_back(n);
        } else if (tag == "f") {
            std::vector<std::pair<int, int>> face;
            std::string token;
            while (stream >> token) {
                const size_t slash1 = token.find('/');
                const size_t slash2 = slash1 == std::string::npos ? slash1 : token.find('/', slash1 + 1);
                const std::string vi = token.substr(0, slash1);
                const std::string ni = slash2 == std::string::npos ? "" : token.substr(slash2 + 1);
                int vertex = -1, normal = -1;
                if (!parseIndex(vi, static_cast<int>(mesh.verts.size()), vertex)) continue;
                if (!ni.empty()) parseIndex(ni, static_cast<int>(mesh.normals.size()), normal);
                face.emplace_back(vertex, normal);
            }
            if (face.size() >= 3) {
                for (size_t i = 0; i < face.size(); ++i)
                    mesh.edges.emplace_back(face[i].first, face[(i + 1) % face.size()].first);
                mesh.faces.push_back(std::move(face));
            }
        }
    }
    return mesh;
}

GLRenderer::GLRenderer(SDL_Window* target, int width, int height) : window(target), context(nullptr) {
    context = SDL_GL_CreateContext(window);
    if (!context) return;
    SDL_GL_SetSwapInterval(1);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    const double nearPlane = 0.1, farPlane = 100.0;
    const double top = nearPlane * std::tan(45.0 * 3.141592653589793 / 360.0);
    glFrustum(-top * width / height, top * width / height, -top, top, nearPlane, farPlane);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);
    glClearColor(0.08f, 0.12f, 0.18f, 1.0f);
    const GLfloat ambient[] = {0.28f, 0.28f, 0.30f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
}

GLRenderer::~GLRenderer() { if (context) SDL_GL_DeleteContext(context); }

void GLRenderer::beginFrame(const Mat4& view, float sunX, float sunY, float sunZ) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    loadMatrix(view);
    // Directional light: w=0; the vector is transformed by the current view matrix.
    GLfloat light[] = {sunX, sunY, sunZ, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light);
}

void GLRenderer::drawMesh(const Mesh& mesh, const Mat4& model, float red, float green, float blue) {
    glPushMatrix();
    GLfloat data[16];
    matrixData(model, data);
    glMultMatrixf(data);
    glColor3f(red, green, blue);
    for (const auto& face : mesh.faces) {
        // Fan triangulation supports convex Blender polygons and keeps face normals intact.
        for (size_t i = 1; i + 1 < face.size(); ++i) {
            const std::pair<int,int> corners[] = {face[0], face[i], face[i + 1]};
            const Vector3& a = mesh.verts[corners[0].first];
            const Vector3& b = mesh.verts[corners[1].first];
            const Vector3& c = mesh.verts[corners[2].first];
            const Vector3 ab{b.x-a.x, b.y-a.y, b.z-a.z};
            const Vector3 ac{c.x-a.x, c.y-a.y, c.z-a.z};
            const Vector3 fallback{ab.y*ac.z-ab.z*ac.y, ab.z*ac.x-ab.x*ac.z, ab.x*ac.y-ab.y*ac.x};
            glBegin(GL_TRIANGLES);
            for (const auto& corner : corners) {
                if (corner.second >= 0 && corner.second < static_cast<int>(mesh.normals.size())) {
                    const Vector3& n = mesh.normals[corner.second];
                    glNormal3f(n.x, n.y, n.z);
                } else {
                    glNormal3f(fallback.x, fallback.y, fallback.z);
                }
                const Vector3& v = mesh.verts[corner.first];
                glVertex3f(v.x, v.y, v.z);
            }
            glEnd();
        }
    }
    glPopMatrix();
}

void GLRenderer::drawPoint(const Vector3& position, float red, float green, float blue, float size) {
    glDisable(GL_LIGHTING);
    glColor3f(red, green, blue);
    glPointSize(size);
    glBegin(GL_POINTS);
    glVertex3f(position.x, position.y, position.z);
    glEnd();
    glEnable(GL_LIGHTING);
}

void GLRenderer::endFrame() { SDL_GL_SwapWindow(window); }
