#pragma once
#include <cmath>

enum Season { SPRING, SUMMER, AUTUMN, WINTER };

struct Color3 { float r, g, b; };
struct Dir3   { float x, y, z; };

struct SeasonState {
    Season season;
    float  progress;      // 0..1 within current season
    float  canopyScale;   // 0..1
    Color3 canopyColor;   // 0..1 floats
    Color3 groundColor;   // 0..1 floats
    Dir3   sunDir;        // normalized
};

const float SEASON_LEN = 5.0f; // seconds per season

inline float lerpf(float a, float b, float t) { return a + (b - a) * t; }

inline Color3 lerpColor(Color3 a, Color3 b, float t) {
    return { lerpf(a.r, b.r, t), lerpf(a.g, b.g, t), lerpf(a.b, b.b, t) };
}

inline SeasonState getSeasonState(float elapsed) {
    const Color3 LIGHT_GREEN = {0.55f, 0.85f, 0.35f};
    const Color3 DEEP_GREEN  = {0.10f, 0.50f, 0.15f};
    const Color3 ORANGE      = {0.90f, 0.45f, 0.10f};
    const Color3 BROWN       = {0.45f, 0.28f, 0.12f};
    const Color3 GRASS       = {0.25f, 0.55f, 0.20f};
    const Color3 DRY_GRASS   = {0.55f, 0.50f, 0.20f};
    const Color3 SOIL        = {0.40f, 0.30f, 0.18f};
    const Color3 SNOW        = {0.95f, 0.97f, 1.00f};
    const float  SUN_Y[4]    = {0.6f, 0.9f, 0.6f, 0.25f}; // sun height per season

    float cycle = fmodf(elapsed, 4.0f * SEASON_LEN);
    int   s = (int)(cycle / SEASON_LEN);
    float p = fmodf(cycle, SEASON_LEN) / SEASON_LEN;

    SeasonState st;
    st.season   = (Season)s;
    st.progress = p;

    switch (st.season) {
    case SPRING: // snow melts, leaves grow back
        st.canopyScale = lerpf(0.2f, 1.0f, p);
        st.canopyColor = lerpColor(BROWN, LIGHT_GREEN, p);
        st.groundColor = lerpColor(SNOW, GRASS, p);
        break;
    case SUMMER:
        st.canopyScale = 1.0f;
        st.canopyColor = lerpColor(LIGHT_GREEN, DEEP_GREEN, p);
        st.groundColor = lerpColor(GRASS, DRY_GRASS, p);
        break;
    case AUTUMN: // green -> orange -> brown
        st.canopyScale = lerpf(1.0f, 0.8f, p);
        st.canopyColor = (p < 0.5f) ? lerpColor(DEEP_GREEN, ORANGE, p * 2.0f)
                                    : lerpColor(ORANGE, BROWN, (p - 0.5f) * 2.0f);
        st.groundColor = lerpColor(DRY_GRASS, SOIL, p);
        break;
    case WINTER: // leaves drop, snow covers ground
        st.canopyScale = lerpf(0.8f, 0.2f, p);
        st.canopyColor = BROWN;
        st.groundColor = lerpColor(SOIL, SNOW, p);
        break;
    }

    // Sun height blends into next season; x/z fixed
    float y = lerpf(SUN_Y[s], SUN_Y[(s + 1) % 4], p);
    float x = -0.35f, z = -0.45f;
    float len = sqrtf(x * x + y * y + z * z);
    st.sunDir = { x / len, y / len, z / len };

    return st;
}
