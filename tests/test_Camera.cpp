#include <catch2/catch_all.hpp>
using namespace Catch::Matchers;

#include "Camera.hpp"
#include "Geometry.hpp"
#include "Math.hpp"

#include <iostream>

TEST_CASE("position", "[Camera]") {

    PerspectiveCamera cam(degToRad(90), 16/9.0f, 0.1f, 100.0f);
    REQUIRE(cam.get_position().isApprox(Eigen::Vector3f(0, 0, 0)));
    REQUIRE(cam.get_up().isApprox(Eigen::Vector3f(0, 1, 0)));
    REQUIRE(cam.get_right().isApprox(Eigen::Vector3f(1, 0, 0)));

    cam.move({0, 0, 1}); // move forward (towards z)
    cam.set_rotation({0, M_PI, 0}); // do a U-turn

    REQUIRE(cam.get_direction().isApprox(Eigen::Vector3f(0, 0, -1)));
    REQUIRE(cam.get_up().isApprox(Eigen::Vector3f(0, 1, 0))); // up is still up
    REQUIRE(cam.get_right().isApprox(Eigen::Vector3f(-1, 0, 0))); // right has changed due to rotation

    float width = abs(cam.get_screen_top_right().x() - cam.get_screen_top_left().x());
    REQUIRE_THAT(width, WithinRel(0.2f));

    float height = abs(cam.get_screen_top_left().y() - cam.get_screen_bottom_left().y());
    //std::cout << "height: " << height << std::endl;
    REQUIRE_THAT(height, WithinRel(width * 9/16.0f));

    //std::cout << "top left: " << cam.get_screen_top_left().transpose() << std::endl;
    //std::cout << "top right: " << cam.get_screen_top_right().transpose() << std::endl;
    //std::cout << "bottom left: " << cam.get_screen_bottom_left().transpose() << std::endl;
    //std::cout << "bottom right: " << cam.get_screen_bottom_right().transpose() << std::endl;

    REQUIRE(cam.get_screen_top_left().isApprox(Eigen::Vector3f(0.1, 0.05625, 0.9)));
    REQUIRE(cam.get_screen_top_right().isApprox(Eigen::Vector3f(-0.1, 0.05625, 0.9)));
    REQUIRE(cam.get_screen_bottom_left().isApprox(Eigen::Vector3f(0.1, -0.05625, 0.9)));
    REQUIRE(cam.get_screen_bottom_right().isApprox(Eigen::Vector3f(-0.1, -0.05625, 0.9)));
}


TEST_CASE("projection", "[Camera]") {
    PerspectiveCamera cam(degToRad(60), 1.0f, 0.1f, 100.0f);
    cam.set_position({0, 0, 1});
    
    // look at 0,0,0 with up vector 0,1,0 and right vector 1,0,0, so the camera is looking "backwards"
    cam.set_rotation({0, M_PI, 0});

    REQUIRE(cam.get_direction().isApprox(Eigen::Vector3f(0, 0, -1)));
    REQUIRE(cam.get_up().isApprox(Eigen::Vector3f(0, 1, 0)));
    REQUIRE(cam.get_right().isApprox(Eigen::Vector3f(-1, 0, 0)));

    auto proj = cam.get_projection();
    
    auto point = Eigen::Vector3f(0, 0, -1); // point in front of the camera, on the z axis, at a distance of 2
    auto projected = proj * point.homogeneous();
    REQUIRE_THAT(projected.x(), WithinRel(0.0f));
    REQUIRE_THAT(projected.y(), WithinRel(0.0f));
    float z1 = projected.z();
    REQUIRE(z1 > 0); // the projected point should be in front of the camera (z > 0)
    auto point2 = Eigen::Vector3f(0, 0, -2); // point in front of the camera, on the z axis, at a distance of 3
    auto projected2 = proj * point2.homogeneous();
    REQUIRE_THAT(projected2.x(), WithinRel(0.0f));
    REQUIRE_THAT(projected2.y(), WithinRel(0.0f));
    float z2 = projected2.z();
    REQUIRE(z2 > 0); // the projected point should be in front of the camera (z > 0)
    REQUIRE(z1 < z2); // the projected point should be closer to the camera than the second point
}

TEST_CASE("get_ray", "[Camera]") {

    PerspectiveCamera cam(degToRad(60.0f), 1.0f, 0.1f, 100.0f);
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
        //std::cout << "ray.direction: " << ray.direction.transpose() << std::endl;
        //std::cout << "vec: " << vec.transpose() << std::endl;
        REQUIRE(ray.direction.norm() == 1.0); // direction should be normalized
        REQUIRE(ray.direction.isApprox(vec));
    }

    SECTION("bottomright ray") {
        auto ray = cam.get_ray(1.0f, 1.0f);
        auto vec = cam.get_screen_bottom_right() - cam.get_position();
        REQUIRE(ray.direction.isApprox(vec.normalized()));
    }
}
