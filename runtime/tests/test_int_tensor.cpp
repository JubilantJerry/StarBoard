#include <iostream>
#include <utility>
#include "custom_utility.hpp"

#include "catch.hpp"
#include "native_interface.hpp"

TEST_CASE("Create integer tensor", "[native_interface]") {
    SECTION("Default construction") {
        IntTensorObj intTensor{};
        REQUIRE(intTensor.getNumSizes() == 0);
    }

    SECTION("Size 2 tensor") {
        IntTensorObj intTensor{(NumSizes){1}, 2};
        IntTensorStruct *intTensorStruct = &intTensor;

        SECTION("Tensor check size") {
            REQUIRE(intTensor.getNumSizes() == 1);
            REQUIRE(intTensor.getSizes()[0] == 2);
            REQUIRE(intTensorStruct->numSizes == 1);
            REQUIRE(intTensorStruct->sizes[0] == 2);
        }

        SECTION("Write & read") {
            int *d = intTensorStruct->contents;
            d[0] = 5;
            d[1] = 6;
            REQUIRE(d[0] == 5);
            REQUIRE(d[1] == 6);
        }
    }

    SECTION("Size (2, 3) tensor") {
        IntTensorObj intTensor{(NumSizes){2}, 2, 3};
        IntTensorStruct *intTensorStruct = &intTensor;

        SECTION("Tensor check size") {
            REQUIRE(intTensor.getNumSizes() == 2);
            REQUIRE(intTensor.getSizes()[0] == 2);
            REQUIRE(intTensor.getSizes()[1] == 3);
            REQUIRE(intTensorStruct->numSizes == 2);
            REQUIRE(intTensorStruct->sizes[0] == 2);
            REQUIRE(intTensorStruct->sizes[1] == 3);
        }

        SECTION("Write & read") {
            int *s = intTensorStruct->sizes;
            int *d = intTensorStruct->contents;
            d[I(s[1], (0), 0)] = 0;
            d[I(s[1], (0), 1)] = 1;
            d[I(s[1], (0), 2)] = 2;
            d[I(s[1], (1), 0)] = 3;
            d[I(s[1], (1), 1)] = 4;
            d[I(s[1], (1), 2)] = 5;
            for (int i = 0; i < 6; i++) {
                REQUIRE(d[i] == i);
            }
        }
    }
}

TEST_CASE("Create float tensor", "[native_interface]") {
    SECTION("Default construction") {
        FloatTensorObj floatTensor{};
        REQUIRE(floatTensor.getNumSizes() == 0);
    }

    SECTION("Size (2, 2, 2) tensor") {
        FloatTensorObj floatTensor{(NumSizes){3}, 2, 2, 2};
        FloatTensorStruct *floatTensorStruct = &floatTensor;

        SECTION("Tensor check size") {
            REQUIRE(floatTensor.getNumSizes() == 3);
            REQUIRE(floatTensor.getSizes()[0] == 2);
            REQUIRE(floatTensor.getSizes()[1] == 2);
            REQUIRE(floatTensor.getSizes()[2] == 2);
            REQUIRE(floatTensorStruct->numSizes == 3);
            REQUIRE(floatTensorStruct->sizes[0] == 2);
            REQUIRE(floatTensorStruct->sizes[1] == 2);
            REQUIRE(floatTensorStruct->sizes[2] == 2);
        }

        SECTION("Write & read") {
            int *s = floatTensorStruct->sizes;
            float *d = floatTensorStruct->contents;
            d[I(s[2], I(s[1], (0), 0), 0)] = 0.0;
            d[I(s[2], I(s[1], (0), 0), 1)] = 0.1;
            d[I(s[2], I(s[1], (0), 1), 0)] = 0.2;
            d[I(s[2], I(s[1], (0), 1), 1)] = 0.3;
            d[I(s[2], I(s[1], (1), 0), 0)] = 0.4;
            d[I(s[2], I(s[1], (1), 0), 1)] = 0.5;
            d[I(s[2], I(s[1], (1), 1), 0)] = 0.6;
            d[I(s[2], I(s[1], (1), 1), 1)] = 0.7;
            for (int i = 0; i < 8; i++) {
                REQUIRE(d[i] == (float) i / 10);
            }
        }
    }
}

TEST_CASE("Access data block", "[native_interface]") {
    DataBlock block{1, 0, 1};
    DataPtr t = make_unique<IntTensorObj>((NumSizes){2}, 1, 1);
    block.setInput(0, std::move(t));
    DataBlock *blockPtr = &block;

    SECTION("Access input values") {
        IntTensorStruct const * intTensor;
        intTensor = getInputIntTensor(blockPtr, 0);
        REQUIRE(intTensor->numSizes == 2);
        REQUIRE(intTensor->sizes[0] == 1);

        intTensor->contents[0] = 5;
        intTensor = getInputIntTensor(blockPtr, 0);
        REQUIRE(intTensor->contents[0] == 5);
    }
}