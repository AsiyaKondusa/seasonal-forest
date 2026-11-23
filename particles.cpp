#include "particles.h"
#include <cstdlib>
#include <cmath>

extern void drawEdge3D(SDL_Renderer* r, Vector3 a, Vector3 b);

namespace {
    float randRange(float minVal, float maxVal) {
        float t = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        return minVal + t * (maxVal - minVal);
    }

    Mat4 rotationX(float angle) {
        Mat4 r = Mat4::identity();
        r.m[1][1] =  cosf(angle);
        r.m[1][2] = -sinf(angle);
        r.m[2][1] =  sinf(angle);
        r.m[2][2] =  cosf(angle);
        return r;
    }
}

ParticleSystem::ParticleSystem() {
    pool.resize(POOL_SIZE);
}

void ParticleSystem::init(const std::vector<Vector3>& canopyCenters, const std::vector<float>& canopyRadii) {
    std::srand(42);
    treeCenters = canopyCenters;
    treeRadii   = canopyRadii;

    leafMesh = loadOBJ("models/leaf.obj");
    snowflakeMesh = loadOBJ("models/snowflake.obj");
    hasLeafMesh = (!leafMesh.verts.empty() && !leafMesh.faces.empty());
    hasSnowflakeMesh = (!snowflakeMesh.verts.empty() && !snowflakeMesh.faces.empty());

    for (auto& p : pool) {
        p.active = false;
    }
}

void ParticleSystem::respawnParticle(Particle& p, Season season, bool initialSpread) {
    p.swayPhase   = randRange(0.0f, 6.28318f);
    p.rotX        = randRange(0.0f, 6.28318f);
    p.rotY        = randRange(0.0f, 6.28318f);
    p.groundTimer = 0.0f;
    p.active      = true;

    if (season == AUTUMN) {
        // Shed leaves from inside/under the actual tree canopies
        if (!treeCenters.empty()) {
            int idx = std::rand() % static_cast<int>(treeCenters.size());
            Vector3 c = treeCenters[idx];
            float r = (idx < static_cast<int>(treeRadii.size())) ? treeRadii[idx] : 0.8f;
            float angle = randRange(0.0f, 6.28318f);
            float dist  = randRange(0.15f, r);

            p.position.x = c.x + std::cos(angle) * dist;
            p.position.z = c.z + std::sin(angle) * dist;
            p.position.y = initialSpread
                ? randRange(0.05f, c.y + 0.3f)
                : c.y + randRange(-0.35f, 0.35f);
        } else {
            p.position.x = randRange(-4.5f, 4.5f);
            p.position.z = randRange(0.5f, 9.5f);
            p.position.y = initialSpread ? randRange(0.2f, 3.0f) : randRange(2.0f, 3.2f);
        }

        // Leaf physics: wider fluttering glide and full 3D tumble
        p.velocity  = { 0.0f, -randRange(0.55f, 1.05f), 0.0f };
        p.swaySpeed = randRange(2.0f, 3.6f);
        p.swayAmp   = randRange(0.55f, 1.15f);
        p.spinX     = randRange(-3.2f, 3.2f);
        p.spinY     = randRange(-2.8f, 2.8f);
        p.scale     = randRange(0.65f, 1.05f); // leaf.obj is 0.36 wide -> ~0.23-0.37 world units

        float tone = randRange(0.0f, 1.0f);
        if (tone < 0.45f) {
            p.color = { 0.92f, 0.38f, 0.06f }; // vivid autumn orange
        } else if (tone < 0.80f) {
            p.color = { 0.88f, 0.62f, 0.10f }; // golden amber
        } else {
            p.color = { 0.58f, 0.22f, 0.07f }; // russet brown
        }
    } else if (season == WINTER) {
        // Snow falls evenly from the sky across the whole forest
        p.position.x = randRange(-4.9f, 4.9f);
        p.position.z = randRange(0.1f, 9.9f);
        p.position.y = initialSpread ? randRange(0.2f, 5.5f) : randRange(4.2f, 5.8f);

        p.velocity  = { 0.0f, -randRange(0.35f, 0.75f), 0.0f };
        p.swaySpeed = randRange(1.0f, 2.0f);
        p.swayAmp   = randRange(0.15f, 0.40f);
        p.spinX     = randRange(-0.8f, 0.8f);
        p.spinY     = randRange(-1.5f, 1.5f);
        p.scale     = randRange(0.45f, 0.75f); // snowflake.obj is 0.24 wide -> ~0.11-0.18 star
        p.color     = { 0.96f, 0.98f, 1.00f }; // crisp white
    } else {
        p.active = false;
    }
}

void ParticleSystem::updateParticles(float dt, Season season) {
    if (season != AUTUMN && season != WINTER) {
        if (currentSeason == AUTUMN || currentSeason == WINTER) {
            for (auto& p : pool) {
                p.active = false;
            }
        }
        currentSeason = season;
        return;
    }

    if (currentSeason != season) {
        currentSeason = season;
        for (auto& p : pool) {
            respawnParticle(p, season, true);
        }
        return;
    }

    for (auto& p : pool) {
        if (!p.active) {
            respawnParticle(p, season, true);
            continue;
        }

        // Fallen leaves briefly rest on the ground before recycling back to the canopy
        if (p.groundTimer > 0.0f) {
            p.groundTimer -= dt;
            if (p.groundTimer <= 0.0f) {
                respawnParticle(p, season, false);
            }
            continue;
        }

        p.swayPhase += p.swaySpeed * dt;
        p.rotX      += p.spinX * dt;
        p.rotY      += p.spinY * dt;

        p.position.y += p.velocity.y * dt;
        p.position.x += std::sin(p.swayPhase) * p.swayAmp * dt;
        p.position.z += std::cos(p.swayPhase * 0.7f) * (p.swayAmp * 0.5f) * dt;

        if (p.position.y <= 0.02f) {
            if (season == AUTUMN) {
                // Lie flat on the forest floor for 1.5 seconds before recycling
                p.position.y  = 0.02f;
                p.rotX        = 0.0f;
                p.groundTimer = randRange(1.0f, 2.2f);
            } else {
                // Snow melts on contact and recycles immediately at the top
                respawnParticle(p, season, false);
            }
        }
    }
}

void ParticleSystem::drawParticles(GLRenderer& gl, Season season) const {
    if (season != AUTUMN && season != WINTER) return;

    const bool useMesh = (season == AUTUMN) ? hasLeafMesh : hasSnowflakeMesh;
    const Mesh& mesh   = (season == AUTUMN) ? leafMesh : snowflakeMesh;

    for (const auto& p : pool) {
        if (!p.active) continue;

        if (useMesh) {
            Mat4 model = translation(p.position.x, p.position.y, p.position.z)
                       * rotationY(p.rotY)
                       * rotationX(p.rotX)
                       * scaling(p.scale, p.scale, p.scale);
            gl.drawMesh(mesh, model, p.color.r, p.color.g, p.color.b);
        } else {
            float ptSize = (season == AUTUMN) ? 6.0f : 4.5f;
            gl.drawPoint(p.position, p.color.r, p.color.g, p.color.b, ptSize);
        }
    }
}

void ParticleSystem::drawParticlesWireframe(SDL_Renderer* renderer, Season season) const {
    if (season != AUTUMN && season != WINTER) return;

    for (const auto& p : pool) {
        if (!p.active) continue;
        float r = p.scale * 0.15f;
        Vector3 a = { p.position.x - r, p.position.y, p.position.z };
        Vector3 b = { p.position.x + r, p.position.y, p.position.z };
        drawEdge3D(renderer, a, b);
    }
}
