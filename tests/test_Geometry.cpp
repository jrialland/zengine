#include <catch2/catch_test_macros.hpp>

#include "Geometry.hpp"

TEST_CASE("quad", "[Geometry]") {
    auto q = basegeometries::quad({0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0});
    REQUIRE(q.vertices.size() == 4);
    REQUIRE(q.indices.size() == 6);
    REQUIRE(q.get_centroid() == Eigen::Vector3f(0.5, 0.5, 0.0));
}

TEST_CASE("cube", "[Geometry]") {
    auto cube = basegeometries::cube();


    std::ostringstream oss;
    cube.to_obj(oss, "cube", true);
    std::cout << oss.str() << std::endl;

    REQUIRE(cube.vertices.size() == 8);
    REQUIRE(cube.indices.size() == 36); // 6 faces * 2 triangles/face * 3 indices/triangle
}
