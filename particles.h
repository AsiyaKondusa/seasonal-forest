#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include "mat4.h"
#include "season.h"
#include "gl_renderer.h"

struct Particle {
    Vector3 position;
    Vector3 velocity;
    float swayPhase;
    float swaySpeed;
    float swayAmp;
    float rotX;
    float rotY;
    float spinX;
    float spinY;
    float scale;
    float groundTimer; // > 0 while resting on ground before recycle
    Color3 color;
    bool active;
};

class ParticleSystem {
public:
    static constexpr int POOL_SIZE = 500;

    ParticleSystem();
    void init(const std::vector<Vector3>& canopyCenters, const std::vector<float>& canopyRadii);
    void updateParticles(float dt, Season season);
    void drawParticles(GLRenderer& gl, Season season) const;
    void drawParticlesWireframe(SDL_Renderer* renderer, Season season) const;

private:
    std::vector<Particle> pool;
    std::vector<Vector3> treeCenters;
    std::vector<float> treeRadii;
    Mesh leafMesh;
    Mesh snowflakeMesh;
    bool hasLeafMesh = false;
    bool hasSnowflakeMesh = false;
    Season currentSeason = SPRING;

    void respawnParticle(Particle& p, Season season, bool initialSpread);
};
