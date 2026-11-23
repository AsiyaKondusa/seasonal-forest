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

    if (season == SPRING) {
        // Gentle cherry blossom petals drifting off blooming canopies
        if (!treeCenters.empty()) {
            int idx = std::rand() % static_cast<int>(treeCenters.size());
            Vector3 c = treeCenters[idx];
            float r = (idx < static_cast<int>(treeRadii.size())) ? treeRadii[idx] : 0.8f;
            float angle = randRange(0.0f, 6.28318f);
            float dist  = randRange(0.15f, r);

            p.position.x = c.x + std::cos(angle) * dist;
            p.position.z = c.z + std::sin(angle) * dist;
            p.position.y = initialSpread
                ? randRange(0.2f, c.y + 0.4f)
                : c.y + randRange(-0.25f, 0.35f);
        } else {
            p.position.x = randRange(-4.5f, 4.5f);
            p.position.z = randRange(0.5f, 9.5f);
            p.position.y = initialSpread ? randRange(0.3f, 3.2f) : randRange(2.0f, 3.2f);
        }

        // Light, breezy float with wide horizontal drift
        p.velocity  = { randRange(0.10f, 0.28f), -randRange(0.25f, 0.55f), 0.0f };
        p.swaySpeed = randRange(1.6f, 2.8f);
        p.swayAmp   = randRange(0.55f, 1.10f);
        p.spinX     = randRange(-2.2f, 2.2f);
        p.spinY     = randRange(-2.0f, 2.0f);
        p.scale     = randRange(0.45f, 0.70f);

        // Soft cherry-blossom pink and white petals
        float tone = randRange(0.0f, 1.0f);
        if (tone < 0.60f) {
            p.color = { 0.98f, 0.68f, 0.80f }; // sakura pink
        } else {
            p.color = { 0.99f, 0.90f, 0.94f }; // blush white
        }
    } else if (season == AUTUMN) {
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
        p.scale     = randRange(0.65f, 1.05f);

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
        p.scale     = randRange(0.45f, 0.75f);
        p.color     = { 0.96f, 0.98f, 1.00f }; // crisp white
    } else {
        // SUMMER: clear warm skies (no falling particles)
        p.active = false;
    }
}

void ParticleSystem::updateParticles(float dt, Season season) {
    if (season == SUMMER) {
        if (currentSeason != SUMMER) {
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

        p.position.x += (p.velocity.x + std::sin(p.swayPhase) * p.swayAmp) * dt;
        p.position.y += p.velocity.y * dt;
        p.position.z += std::cos(p.swayPhase * 0.7f) * (p.swayAmp * 0.5f) * dt;

        if (p.position.y <= 0.02f) {
            if (season == AUTUMN) {
                p.position.y  = 0.02f;
                p.rotX        = 0.0f;
                p.groundTimer = randRange(1.0f, 2.2f);
            } else {
                respawnParticle(p, season, false);
            }
        }
    }
}

void ParticleSystem::drawParticles(GLRenderer& gl, Season season) const {
    if (season == SUMMER) return;

    const bool useMesh = (season == WINTER) ? hasSnowflakeMesh : hasLeafMesh;
    const Mesh& mesh   = (season == WINTER) ? snowflakeMesh : leafMesh;

    for (const auto& p : pool) {
        if (!p.active) continue;

        if (useMesh) {
            Mat4 model = translation(p.position.x, p.position.y, p.position.z)
                       * rotationY(p.rotY)
                       * rotationX(p.rotX)
                       * scaling(p.scale, p.scale, p.scale);
            gl.drawMesh(mesh, model, p.color.r, p.color.g, p.color.b);
        } else {
            float ptSize = (season == WINTER) ? 4.5f : 5.5f;
            gl.drawPoint(p.position, p.color.r, p.color.g, p.color.b, ptSize);
        }
    }
}

void ParticleSystem::drawParticlesWireframe(SDL_Renderer* renderer, Season season) const {
    if (season == SUMMER) return;

    for (const auto& p : pool) {
        if (!p.active) continue;
        float r = p.scale * 0.15f;
        Vector3 a = { p.position.x - r, p.position.y, p.position.z };
        Vector3 b = { p.position.x + r, p.position.y, p.position.z };
        drawEdge3D(renderer, a, b);
    }
}
