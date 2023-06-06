#pragma once

#include <vector>
#include <memory>
#include <string>

#include <Eigen/Dense>

#include "Camera.hpp"
#include "Geometry.hpp"

/**
 * @brief just a simple cpu crude ray tracer for validating the correctness of geometries
 * 
 */
class CpuRayTracer
{
    std::shared_ptr<PerspectiveCamera> camera;

    Eigen::Vector3f sun_direction = Eigen::Vector3f(-1.0f, -1.0f, 1.0f).normalized();

    std::vector<std::shared_ptr<Geometry>> geometries;

    public:

    CpuRayTracer(std::shared_ptr<PerspectiveCamera> camera);

    void add_geometry(const Geometry& geometry, const Eigen::Matrix4f& transform = Eigen::Matrix4f::Identity());

    void set_sun_direction(const Eigen::Vector3f& sun_direction);

    void set_camera(std::shared_ptr<PerspectiveCamera> camera);

    std::shared_ptr<PerspectiveCamera> get_camera() const;

    void render(const std::string& filename, int width, int height, int samples = 1);
};