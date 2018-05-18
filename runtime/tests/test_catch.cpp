#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "test_lib.hpp"

TEST_CASE("Example unit test", "[test_catch]") {
    REQUIRE(someNumber == 1337);
}