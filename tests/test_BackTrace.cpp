#include <catch2/catch_all.hpp>
#include "BackTrace.hpp"

TEST_CASE("backtrace", "[BackTrace]") {
    print_backtrace(0);
}