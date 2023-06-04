#include <catch2/catch_test_macros.hpp>
#include "Md5.hpp"
#include <sstream>

TEST_CASE("Md5", "[Md5]")
{
    SECTION("hexdigest")
    {
        REQUIRE(Md5Digest::hexdigest("Hello World") == "b10a8db164e0754105b7a99be72e3fe5");
    }

    SECTION("hexdigest stream")
    {
        std::stringstream stream;
        stream << "Hello World";

        REQUIRE(Md5Digest::hexdigest(stream) == "b10a8db164e0754105b7a99be72e3fe5");
    }

    SECTION("hexdigest empty")
    {
        REQUIRE(Md5Digest::hexdigest("") == "d41d8cd98f00b204e9800998ecf8427e");
    }
}