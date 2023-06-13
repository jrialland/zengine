#include <catch2/catch_all.hpp>
using namespace Catch::Matchers;

#include "Camera.hpp"
#include "Geometry.hpp"
#include "Math.hpp"

#include <iostream>


TEST_CASE("get_ray", "[Camera]") {

    PerspectiveCamera cam(60.0_deg, 1.0f, 0.1f, 100.0f);
    cam.set_position({0, 0, 1});
    cam.look_at({0, 0, 0});

    SECTION("middle ray") {
        auto ray = cam.get_ray(0.5f, 0.5f);
        REQUIRE(ray.direction.isApprox(cam.get_direction()));
        REQUIRE(ray.origin.isApprox(cam.get_position()));
    }

    SECTION("topleft ray") {
        auto ray = cam.get_ray(0.0f, 0.0f);
        auto vec = (cam.get_screen_top_left() - cam.get_position()).normalized();
        REQUIRE(ray.direction.norm() == 1.0); // direction should be normalized
        REQUIRE(ray.direction.isApprox(vec));
    }

    SECTION("bottomright ray") {
        auto ray = cam.get_ray(1.0f, 1.0f);
        auto vec = cam.get_screen_bottom_right() - cam.get_position();
        REQUIRE(ray.direction.isApprox(vec.normalized()));
    }
}

TEST_CASE("projection", "[Camera]") {
    PerspectiveCamera cam(45.0_deg, 16/9.0, 0.1f, 100.0f);
    Eigen::Matrix4f proj = cam.get_projection();
    std::cout << proj << std::endl;
    Eigen::Matrix4f expected;
    expected <<
    0.5625  ,0          ,0          ,0,
    0          ,1 ,0          ,0,
    0          ,0         ,-1.002   ,-1,
    0          ,0         ,-0.2002   ,0;
    REQUIRE(proj.isApprox(expected));
}