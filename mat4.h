#pragma once
#include <cmath>

struct Vector3 { float x, y, z; };

struct Mat4 {
    float m[4][4];

    static Mat4 identity() {
        Mat4 r{};
        for (int i = 0; i < 4; i++) r.m[i][i] = 1.0f;
        return r;
    }
};

inline Mat4 operator*(const Mat4& a, const Mat4& b) {
    Mat4 r{};
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            for (int k = 0; k < 4; k++)
                r.m[i][j] += a.m[i][k] * b.m[k][j];
    return r;
}

inline Vector3 transform(const Mat4& M, const Vector3& v) {
    return {
        M.m[0][0]*v.x + M.m[0][1]*v.y + M.m[0][2]*v.z + M.m[0][3],
        M.m[1][0]*v.x + M.m[1][1]*v.y + M.m[1][2]*v.z + M.m[1][3],
        M.m[2][0]*v.x + M.m[2][1]*v.y + M.m[2][2]*v.z + M.m[2][3]
    };
}

inline Mat4 translation(float tx, float ty, float tz) {
    Mat4 r = Mat4::identity();
    r.m[0][3] = tx; r.m[1][3] = ty; r.m[2][3] = tz;
    return r;
}

inline Mat4 scaling(float sx, float sy, float sz) {
    Mat4 r = Mat4::identity();
    r.m[0][0] = sx; r.m[1][1] = sy; r.m[2][2] = sz;
    return r;
}

inline Mat4 rotationY(float angle) {
    Mat4 r = Mat4::identity();
    r.m[0][0] =  cosf(angle);
    r.m[0][2] =  sinf(angle);
    r.m[2][0] = -sinf(angle);
    r.m[2][2] =  cosf(angle);
    return r;
}
