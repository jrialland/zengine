#pragma once
#include <Eigen/Dense>
#include <cmath>

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