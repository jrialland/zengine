#include <catch2/catch_all.hpp>
using namespace Catch::Matchers;

#include "Math.hpp"

#define ALMOST_EQUALS(a) WithinRel(float(a), 0.0001f)

TEST_CASE("degToRad", "[Math]")
{
    REQUIRE_THAT(degToRad(0.0f), ALMOST_EQUALS(0.0f));
    REQUIRE_THAT(degToRad(90.0f), ALMOST_EQUALS(M_PI_2));
    REQUIRE_THAT(degToRad(180.0f), ALMOST_EQUALS(M_PI));
    REQUIRE_THAT(degToRad(270.0f), ALMOST_EQUALS(3.0f * M_PI_2));
    REQUIRE_THAT(degToRad(360.0f), ALMOST_EQUALS(2.0f * M_PI));
}

TEST_CASE("radToDeg", "[Math]")
{
    auto [a, expected] = GENERATE(table<float, float>({
        { 0.0f,     0.0f },
        { M_PI_2,   90.0f },
        { M_PI,     180.0f },
        { 3*M_PI_2, 270.0f },
        { 2*M_PI, 360.0f },
    }));
    CAPTURE(a);
    REQUIRE_THAT(radToDeg(a), ALMOST_EQUALS(expected));
}

TEST_CASE("cartesianToSpherical", "[Math]") {
    Eigen::Vector3f v(0.5,0.5,0.5);
    Eigen::Vector3f rtp = cartesianToSpherical(v);
    REQUIRE_THAT(rtp[0], ALMOST_EQUALS(0.866025f));
    REQUIRE_THAT(rtp[1], ALMOST_EQUALS(0.955317f));
    REQUIRE_THAT(rtp[2], ALMOST_EQUALS(0.785398f));
}

TEST_CASE("sphericalToCartesian", "[Math]") {
    Eigen::Vector3f rtp(0.866025f, 0.955317f, 0.785398f);
    Eigen::Vector3f v = sphericalToCartesian(rtp);
    REQUIRE_THAT(v[0], ALMOST_EQUALS(0.5f));
    REQUIRE_THAT(v[1], ALMOST_EQUALS(0.5f));
    REQUIRE_THAT(v[2], ALMOST_EQUALS(0.5f));
}
