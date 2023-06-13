#include <catch2/catch_test_macros.hpp>

#include "CpuRayTracer.hpp"

TEST_CASE("CpuRayTracer", "[CpuRayTracer]") {

    Geometry cube = basegeometries::cube();

    std::shared_ptr<PerspectiveCamera> camera = std::make_shared<PerspectiveCamera>();
    camera->set_position(Eigen::Vector3f(3, 3, 3));
    camera->look_at(Eigen::Vector3f(0.0f, 0.0f, 0.0f));

    CpuRayTracer ray_tracer(camera);
    ray_tracer.add_geometry(cube);

    //ray_tracer.render("test.pgm", 512, 512);

}