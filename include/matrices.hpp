#include <Eigen/Dense>
#include <Eigen/src/Core/Map.h>
#include <Eigen/src/Core/Matrix.h>
#include <cmath>

#include "common_types.h"

Point3f m4_cross_v3(Eigen::Matrix4f mat, Point3f vec) {
    Point4f tmp{vec(0), vec(1), vec(2), 1};
    auto cross = mat * tmp;
    return {cross(0), cross(1), cross(2)};
}

Eigen::Matrix4f scale_matrix(float x, float y, float z) {
    return Eigen::Matrix4f{
        {x, 0, 0, 1}, {0, y, 0, 1}, {0, 0, z, 1}, {0, 0, 0, 1}};
}

Eigen::Matrix4f move_matrix(float x, float y, float z) {
    return Eigen::Matrix4f{
        {0, 0, 0, x}, {0, 0, 0, y}, {0, 0, 0, z}, {0, 0, 0, 1}};
}

Eigen::Matrix4f rotation_matrix(float x, float y, float z) {
    auto rx = Eigen::Matrix4f{{1, 0, 0, 0},
                              {0, cosf(x), -sinf(x), 0},
                              {0, sinf(x), cosf(x), 0},
                              {0, 0, 0, 1}};
    auto ry = Eigen::Matrix4f{{cosf(y), 0, sinf(y), 0},
                              {0, 1, 0, 0},
                              {-sinf(y), 0, cosf(y), 0},
                              {0, 0, 0, 1}};
    auto rz = Eigen::Matrix4f{{cosf(z), -sinf(z), 0, 0},
                              {sinf(z), cosf(z), 0, 0},
                              {0, 0, 1, 0},
                              {0, 0, 0, 1}};
    return rx * ry * rz;
}

Eigen::Matrix4f look_at_camera_matrix(Point3f camera, Point3f target,
                                      Point3f up) {
    Point3f z = (camera - target).normalized();
    Point3f x = up.cross(z).normalized();
    Point3f y = z.cross(x);

    auto orient = Eigen::Matrix4f{{x(0), y(0), z(0), 0},
                                  {x(1), y(1), z(1), 0},
                                  {x(2), y(2), z(2), 0},
                                  {0, 0, 0, 1}};

    auto move = Eigen::Matrix4f{{1, 0, 0, 0},
                                {0, 1, 0, 0},
                                {0, 0, 1, 0},
                                {-camera(0), -camera(1), -camera(2), 1}};

    return move * orient;
}

Eigen::Matrix4f horizontal_fov_projection_matrix(float fovX, float aspectRatio,
                                                 float front, float back) {
    const float DEG2RAD = acos(-1.0f) / 180;

    float tangent = tan(fovX / 2 * DEG2RAD); // tangent of half fovX
    float right   = front * tangent;         // half width of near plane
    float top     = right / aspectRatio;     // half height of near plane

    // params: left, right, bottom, top, near(front), far(back)
    Eigen::Matrix4f matrix;
    matrix.fill(0);
    matrix(0, 0) = front / right;
    matrix(1, 1) = front / top;
    matrix(2, 2) = -(back + front) / (back - front);
    matrix(2, 3) = -1;
    matrix(3, 2) = -(2 * back * front) / (back - front);
    matrix(3, 3) = 1;
    return matrix;
}
