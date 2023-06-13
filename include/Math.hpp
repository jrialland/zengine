#pragma once
#include <Eigen/Dense>
#include <cmath>
#include <algorithm>

typedef float angle_t;

inline constexpr float deg_to_rad(float deg) {
    return deg * M_PI / 180.0f;
}

inline constexpr float rad_to_deg(float rad) {
    return rad * 180.0f / M_PI;
}

inline constexpr angle_t operator"" _deg(long double deg) {
    return deg_to_rad(static_cast<float>(deg));
}

inline constexpr angle_t operator"" _deg(unsigned long long deg) {
    return deg_to_rad(static_cast<float>(deg));
}

inline constexpr angle_t operator"" _rad(long double rad) {
    return static_cast<float>(rad);
}

inline constexpr angle_t operator"" _rad(unsigned long long rad) {
    return static_cast<float>(rad);
}

inline Eigen::Vector3f cartesian_to_spherical(const Eigen::Vector3f &xyz) {
    float r = xyz.norm();
    float theta = acos(xyz.z() / r);
    float phi = atan2(xyz.y(), xyz.x());
    return Eigen::Vector3f(r, theta, phi);
}

inline Eigen::Vector3f spherical_to_cartesian(const Eigen::Vector3f &rtp) {
    float x = rtp.x() * sin(rtp.y()) * cos(rtp.z());
    float y = rtp.x() * sin(rtp.y()) * sin(rtp.z());
    float z = rtp.x() * cos(rtp.y());
    return Eigen::Vector3f(x, y, z);
}

inline Eigen::Matrix4f create_perspective_projection_from_bounds(float left, float right, float bottom, float top, float near, float far) {
    float A = (right + left) / (right - left);
    float B = (top + bottom) / (top - bottom);
    float C = -(far + near) / (far - near);
    float D = -(2 * far * near) / (far - near);
    float E = (2 * near) / (right - left);
    float F = (2 * near) / (top - bottom);
    return Eigen::Matrix4f({
            {E, 0, 0, 0},
            {0, F, 0, 0},
            {A, B, C, -1},
            {0, 0, D, 0}
    });
}

inline Eigen::Matrix4f create_perspective_projection(angle_t fovy, float aspect, float near, float far) {
    float ymax = near * tan(fovy);
    float xmax = ymax * aspect;
    return create_perspective_projection_from_bounds(-xmax, xmax, -ymax, ymax, near, far); 
}

inline Eigen::Matrix4f create_orthographic_projection(float left, float right, float bottom, float top, float near, float far) {
    float A = 2 / (right - left);
    float B = 2 / (top - bottom);
    float C = -2 / (far - near);
    float Tx = -(right + left) / (right - left);
    float Ty = -(top + bottom) / (top - bottom);
    float Tz = -(far + near) / (far - near);
    return Eigen::Matrix4f({
        {A, 0, 0, 0},
        {0, B, 0, 0},
        {0, 0, C, 0},
        {Tx, Ty, Tz, 1}
    });
}

inline Eigen::Vector3f rotate(const Eigen::Vector3f &axis, angle_t angle, const Eigen::Vector3f &v) {
    return Eigen::AngleAxisf(angle, axis) * v;
}

/**
 * @brief Create a look at matrix
 * 
 * @param eye the position of the camera
 * @param target the target point
 * @param up The up vector is passed by reference and will be modified to be orthogonal to the forward vector
 * @return Eigen::Matrix4f 
 */
inline Eigen::Matrix4f look_at(const Eigen::Vector3f &eye, const Eigen::Vector3f &target, const Eigen::Vector3f &up) {
    Eigen::Vector3f forward = (target - eye).normalized();
    Eigen::Vector3f side = forward.cross(up).normalized();
    Eigen::Vector3f new_up = side.cross(forward);
    return Eigen::Matrix4f({
        {side.x(), new_up.x(), -forward.x(), 0},
        {side.y(), new_up.y(), -forward.y(), 0},
        {side.z(), new_up.z(), -forward.z(), 0},
        {-side.dot(eye), -new_up.dot(eye), forward.dot(eye), 1}
    });
}
