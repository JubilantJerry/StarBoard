#include <iostream>
#include <memory>

#include "catch.hpp"
#include "native_interface.hpp"

TEST_CASE("Create integer tensor", "[interface]") {
    DataUnion data;        IntTensorObj intTensor{numSizesArg(1), 2};

    SECTION("Size 2 tensor") {
        IntTensorObj intTensor{numSizesArg(1), 2};
        data.intTensor = &intTensor;

        SECTION("Tensor check size") {
            REQUIRE(intTensor.getNumSizes() == 1);
            REQUIRE(intTensor.getSizes()[0] == 2);
            REQUIRE(data.intTensor->numSizes == 1);
            REQUIRE(data.intTensor->sizes[0] == 2);
        }

        SECTION("Write & read") {
            int *d = data.intTensor->contents;
            d[0] = 5;
            d[1] = 6;
            REQUIRE(d[0] == 5);
            REQUIRE(d[1] == 6);
        }
    }

    SECTION("Size (2, 3) tensor") {
        IntTensorObj intTensor{numSizesArg(2), 2, 3};
        data.intTensor = &intTensor;

        SECTION("Tensor check size") {
            REQUIRE(intTensor.getNumSizes() == 2);
            REQUIRE(intTensor.getSizes()[0] == 2);
            REQUIRE(intTensor.getSizes()[1] == 3);
            REQUIRE(data.intTensor->numSizes == 2);
            REQUIRE(data.intTensor->sizes[0] == 2);
            REQUIRE(data.intTensor->sizes[1] == 3);
        }

        SECTION("Write & read") {
            int *s = data.intTensor->sizes;
            int *d = data.intTensor->contents;
            d[I(s[1], 0, 0)] = 1;
            d[I(s[1], 0, 1)] = 2;
            d[I(s[1], 0, 2)] = 3;
            d[I(s[1], 1, 0)] = 4;
            d[I(s[1], 1, 1)] = 5;
            d[I(s[1], 1, 2)] = 6;
            REQUIRE(d[0] == 1);
            REQUIRE(d[1] == 2);
            REQUIRE(d[2] == 3);
            REQUIRE(d[3] == 4);
            REQUIRE(d[4] == 5);
            REQUIRE(d[5] == 6);
        }
    }
}