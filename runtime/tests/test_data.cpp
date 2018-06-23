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
            int const *s = intTensorStruct->sizes;
            int *d = intTensorStruct->contents;
            d[INDEX(s, 0, 0)] = 0;
            d[INDEX(s, 0, 1)] = 1;
            d[INDEX(s, 0, 2)] = 2;
            d[INDEX(s, 1, 0)] = 3;
            d[INDEX(s, 1, 1)] = 4;
            d[INDEX(s, 1, 2)] = 5;
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
            int const *s = floatTensorStruct->sizes;
            float *d = floatTensorStruct->contents;
            d[INDEX(s, 0, 0, 0)] = 0.0f;
            d[INDEX(s, 0, 0, 1)] = 0.1f;
            d[INDEX(s, 0, 1, 0)] = 0.2f;
            d[INDEX(s, 0, 1, 1)] = 0.3f;
            d[INDEX(s, 1, 0, 0)] = 0.4f;
            d[INDEX(s, 1, 0, 1)] = 0.5f;
            d[INDEX(s, 1, 1, 0)] = 0.6f;
            d[INDEX(s, 1, 1, 1)] = 0.7f;
            for (int i = 0; i < 8; i++) {
                REQUIRE(d[i] == (float) i / 10);
            }
        }
    }
}

TEST_CASE("Create branches", "[native_interface]") {
    SECTION("Empty branch") {
        Branch empty{0};
        REQUIRE(empty.getSize() == 0);
        REQUIRE(getBranchSize(&empty) == 0);
    }

    SECTION("Int / Float tuple as a branch") {
        Branch branch{2};
        Branch *branchPtr;

        {
            std::unique_ptr<IntTensorObj> intScalar;
            std::unique_ptr<FloatTensorObj> floatScalar;

            intScalar = make_unique<IntTensorObj>((NumSizes){1}, 1);
            floatScalar = make_unique<FloatTensorObj>((NumSizes){1}, 1);

            intScalar->getContents()[0] = 5;
            floatScalar->getContents()[0] = 6.0f;

            branch.setValue(0, std::move(intScalar));
            branch.setValue(1, std::move(floatScalar));
            branchPtr = &branch;
        }

        IntTensorStruct const *intScalar;
        FloatTensorStruct const *floatScalar;

        intScalar = getBranchIntTensor(branchPtr, 0);
        floatScalar = getBranchFloatTensor(branchPtr, 1);

        REQUIRE(intScalar->contents[0] == 5);
        REQUIRE(floatScalar->contents[0] == 6.0f);
    }

    SECTION("Branch containing empty branch") {
        Branch branch{2};
        Branch *branchPtr;

        {
            std::unique_ptr<Branch> branchEmpty = make_unique<Branch>(0);

            branch.setValue(0, std::move(branchEmpty));
            branchPtr = &branch;
        }

        Branch *branchEmpty = getBranchBranch(branchPtr, 0);
        REQUIRE(getBranchSize(branchEmpty) == 0);
    }

    SECTION("Modifying branches") {
        Branch branch{1};
        Branch *branchPtr = &branch;

        IntTensorStruct const *intTensor;
        FloatTensorStruct const *floatTensor;
        Branch *branchInner;

        intTensor = makeBranchIntTensor(branchPtr, 0, (NumSizes){1}, 1);
        REQUIRE(intTensor->numSizes == 1);

        floatTensor = makeBranchFloatTensor(branchPtr, 0, (NumSizes){1}, 1);
        REQUIRE(floatTensor->numSizes == 1);

        branchInner = makeBranchBranch(branchPtr, 0, 1);
        REQUIRE(getBranchSize(branchInner) == 1);

        intTensor = makeBranchIntTensor(
            branchPtr, APPEND_INDEX, (NumSizes){2}, 1);
        REQUIRE(intTensor->numSizes == 2);
        REQUIRE(getBranchSize(branchPtr) == 2);
        REQUIRE(getBranchIntTensor(branchPtr, 1) == intTensor);

        floatTensor = makeBranchFloatTensor(
            branchPtr, APPEND_INDEX, (NumSizes){2}, 1);
        REQUIRE(floatTensor->numSizes == 2);
        REQUIRE(getBranchSize(branchPtr) == 3);
        REQUIRE(getBranchFloatTensor(branchPtr, 2) == floatTensor);

        branchInner = makeBranchBranch(
            branchPtr, APPEND_INDEX, 2);
        REQUIRE(getBranchSize(branchInner) == 2);
        REQUIRE(getBranchSize(branchPtr) == 4);
        REQUIRE(getBranchBranch(branchPtr, 3) == branchInner);

        popBranch(branchPtr);
        REQUIRE(getBranchSize(branchPtr) == 3);
    }
}

TEST_CASE("Access data block", "[native_interface]") {
    DataBlock block{3, 0, 1};
    DataBlock *blockPtr;

    {
        std::unique_ptr<IntTensorObj> intTensor;
        std::unique_ptr<FloatTensorObj> floatTensor;
        std::unique_ptr<Branch> branch;

        intTensor = make_unique<IntTensorObj>((NumSizes){2}, 1, 1);

        floatTensor = make_unique<FloatTensorObj>((NumSizes){3}, 1, 1, 1);
        floatTensor->getContents()[0] = 6.0f;

        branch = make_unique<Branch>(0);

        block.setInput(0, std::move(intTensor));
        block.setInput(1, std::move(floatTensor));
        block.setInput(2, std::move(branch));

        blockPtr = &block;
    }

    SECTION("Access block properties") {
        REQUIRE(getNumInputs(blockPtr) == 3);
        REQUIRE(getNumOutputs(blockPtr) == 1);
    }

    SECTION("Access input values") {
        IntTensorStruct const *intTensor;
        FloatTensorStruct const *floatTensor;
        Branch const *branch;

        intTensor = getInputIntTensor(blockPtr, 0);
        REQUIRE(intTensor->numSizes == 2);
        REQUIRE(intTensor->sizes[0] == 1);

        intTensor->contents[0] = 5;
        intTensor = getInputIntTensor(blockPtr, 0);
        REQUIRE(intTensor->contents[0] == 5);

        floatTensor = getInputFloatTensor(blockPtr, 1);
        REQUIRE(floatTensor->contents[0] == 6.0f);

        branch = getInputBranch(blockPtr, 2);
        REQUIRE(getBranchSize(branch) == 0);
    }

    SECTION("Create output values") {
        IntTensorStruct const *intTensor;
        FloatTensorStruct const *floatTensor;
        Branch *branch;

        intTensor = makeOutputIntTensor(blockPtr, 0, (NumSizes){1}, 1);
        REQUIRE(intTensor->numSizes == 1);

        floatTensor = makeOutputFloatTensor(blockPtr, 0, (NumSizes){1}, 1);
        REQUIRE(floatTensor->numSizes == 1);

        branch = makeOutputBranch(blockPtr, 0, 1);
        REQUIRE(getBranchSize(branch) == 1);

        intTensor = moveToOutputIntTensor(blockPtr, 0, 0);
        REQUIRE(intTensor->numSizes == 2);

        floatTensor = moveToOutputFloatTensor(blockPtr, 0, 1);
        REQUIRE(floatTensor->numSizes == 3);

        branch = moveToOutputBranch(blockPtr, 0, 2);
        REQUIRE(getBranchSize(branch) == 0);

        Branch *branchObj = (Branch *)block.takeOutput(0).get();
        REQUIRE(branchObj->getSize() == 0);
    }
}