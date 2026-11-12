#include <iostream>
#include <cmath>

struct Vector3 { float x, y, z; };

struct Mat4 {
    float m[4][4];

    static Mat4 identity() {
        Mat4 r{};                      // {} sets everything to 0
        for (int i = 0; i < 4; i++)
            r.m[i][i] = 1.0f;          // 1s down the diagonal
        return r;
    }
};

// Multiply two matrices: result = a * b
Mat4 operator*(const Mat4& a, const Mat4& b) {
    Mat4 r{};
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            for (int k = 0; k < 4; k++)
                r.m[i][j] += a.m[i][k] * b.m[k][j];
    return r;
}

// Apply a matrix to a 3D point (w is assumed to be 1)
Vector3 transform(const Mat4& M, const Vector3& v) {
    return {
        M.m[0][0]*v.x + M.m[0][1]*v.y + M.m[0][2]*v.z + M.m[0][3],
        M.m[1][0]*v.x + M.m[1][1]*v.y + M.m[1][2]*v.z + M.m[1][3],
        M.m[2][0]*v.x + M.m[2][1]*v.y + M.m[2][2]*v.z + M.m[2][3]
    };
}

Mat4 translation(float tx, float ty, float tz) {
    Mat4 r = Mat4::identity();
    r.m[0][3] = tx;
    r.m[1][3] = ty;
    r.m[2][3] = tz;
    return r;
}

Mat4 scaling(float sx, float sy, float sz) {
    Mat4 r = Mat4::identity();
    r.m[0][0] = sx;
    r.m[1][1] = sy;
    r.m[2][2] = sz;
    return r;
}

// Rotate around the Y axis. angle is in RADIANS.
Mat4 rotationY(float angle) {
    Mat4 r = Mat4::identity();
    r.m[0][0] =  cosf(angle);
    r.m[0][2] =  sinf(angle);
    r.m[2][0] = -sinf(angle);
    r.m[2][2] =  cosf(angle);
    return r;
}
int main() {
    float halfPi = 1.5707963f;

    // Rotate the point (1,0,0) by 90 degrees around Y
    Vector3 a = transform(rotationY(halfPi), {1, 0, 0});
    std::cout << a.x << " " << a.y << " " << a.z << "\n";   // expect: ~0 0 -1

    // Rotating a point on the Y axis should do nothing
    Vector3 b = transform(rotationY(halfPi), {0, 5, 0});
    std::cout << b.x << " " << b.y << " " << b.z << "\n";   // expect: 0 5 0

    // Full tree-style chain: scale, then rotate, then translate
    Mat4 model = translation(4, 0, 2) * rotationY(halfPi) * scaling(2, 2, 2);
    Vector3 c = transform(model, {1, 0, 0});
    std::cout << c.x << " " << c.y << " " << c.z << "\n";   // expect: ~4 0 0
}
