#include <catch2/catch_all.hpp>
using namespace Catch::Matchers;

#include "Math.hpp"

#define ALMOST_EQUALS(a) WithinRel(float(a), 0.0001f)

TEST_CASE("degToRad", "[Math]")
{
    REQUIRE_THAT(0.0_deg, ALMOST_EQUALS(0.0f));
    REQUIRE_THAT(90.0_deg, ALMOST_EQUALS(M_PI_2));
    REQUIRE_THAT(180.0_deg, ALMOST_EQUALS(M_PI));
}

TEST_CASE("cartesian_to_spherical", "[Math]") {
    Eigen::Vector3f v(0.5,0.5,0.5);
    Eigen::Vector3f rtp = cartesian_to_spherical(v);
    REQUIRE_THAT(rtp[0], ALMOST_EQUALS(0.866025f));
    REQUIRE_THAT(rtp[1], ALMOST_EQUALS(0.955317f));
    REQUIRE_THAT(rtp[2], ALMOST_EQUALS(0.785398f));
}

TEST_CASE("spherical_to_cartesian", "[Math]") {
    Eigen::Vector3f rtp(0.866025f, 0.955317f, 0.785398f);
    Eigen::Vector3f v = spherical_to_cartesian(rtp);
    REQUIRE_THAT(v[0], ALMOST_EQUALS(0.5f));
    REQUIRE_THAT(v[1], ALMOST_EQUALS(0.5f));
    REQUIRE_THAT(v[2], ALMOST_EQUALS(0.5f));
}
