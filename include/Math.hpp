#pragma once
#include <Eigen/Dense>
#include <cmath>
#include <algorithm>

inline float degToRad(float deg) {
    return deg * M_PI / 180.0f;
}

inline float radToDeg(float rad) {
    return rad * 180.0f / M_PI;
}

inline Eigen::Vector3f cartesianToSpherical(const Eigen::Vector3f &xyz) {
    float r = xyz.norm();
    float theta = acos(xyz.z() / r);
    float phi = atan2(xyz.y(), xyz.x());
    return Eigen::Vector3f(r, theta, phi);
}

inline Eigen::Vector3f sphericalToCartesian(const Eigen::Vector3f &rtp) {
    float x = rtp.x() * sin(rtp.y()) * cos(rtp.z());
    float y = rtp.x() * sin(rtp.y()) * sin(rtp.z());
    float z = rtp.x() * cos(rtp.y());
    return Eigen::Vector3f(x, y, z);
}

inline Eigen::Vector3f rotate(const Eigen::Vector3f &axis, float angle, const Eigen::Vector3f &v) {
    return Eigen::AngleAxisf(angle, axis) * v;
}

inline float step(float edge, float x) {
    return x < edge ? 0.0f : 1.0f;
}

inline float smoothstep(float a, float b, float t) {
    t = std::clamp((t - a) / (b - a), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

inline float smootherstep(float a, float b, float t) {
    t = std::clamp((t - a) / (b - a), 0.0f, 1.0f);
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

inline float mix(float a, float b, float t) {
    return a * (1.0f - t) + b * t;
}
