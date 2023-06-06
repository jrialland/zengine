#pragma once

#include <Eigen/Dense>

struct Ray {
    public:
    Eigen::Vector3f origin;
    Eigen::Vector3f direction;

    Ray(const Eigen::Vector3f& origin, const Eigen::Vector3f& direction) : origin(origin), direction(direction) {}

    inline Eigen::Vector3f at(float t) const {
        return origin + t * direction;
    }
};