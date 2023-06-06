#include <catch2/catch_all.hpp>
using namespace Catch::Matchers;

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
    //std::cout << oss.str() << std::endl;

    REQUIRE(cube.vertices.size() == 8);
    REQUIRE(cube.indices.size() == 36); // 6 faces * 2 triangles/face * 3 indices/triangle
}

TEST_CASE("hit", "[Geometry]") {
    
    Geometry triangle = basegeometries::triangle({0, 0, 0}, {1, 0, 0}, {0, 1, 0}); // unit triangle in xy plane
    Ray ray{{0.25, 0.25, 1}, {0, 0, -1}}; // ray from (0.25, 0.25, 1) in direction (0, 0, -1), would hit the triangle at (0.25, 0.25, 0)
    float t;
    Eigen::Vector3f normal;
    bool hit = triangle.hit(ray, t, normal);
    REQUIRE(hit);
    REQUIRE_THAT(t, WithinRel(1, 1e-5));
    REQUIRE(ray.at(t).isApprox(Eigen::Vector3f(0.25, 0.25, 0))); // hit point is on the triangle
    REQUIRE(normal.isApprox(Eigen::Vector3f(0, 0, 1))); // normal is pointing up
}
