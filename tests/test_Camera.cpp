#include <catch2/catch_test_macros.hpp>
#include "Camera.hpp"

#include <iostream>

TEST_CASE("projection", "[Camera]") {
    PerspectiveCamera cam(60.0f, 1.0f, 0.1f, 100.0f);
    cam.set_position({0, 0, 1});
    cam.look_at({0, 0, 0});
    auto proj = cam.get_projection();
    std::cout << proj << std::endl;
}
