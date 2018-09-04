#include <iostream>
#include <utility>
#include <custom_utility.hpp>

#include "catch.hpp"
#include "data_interface.hpp"

TEST_CASE("Create integer tensor", "[data_interface]") {
    SECTION("Default construction") {
        IntTensor intTensor{};
        REQUIRE(intTensor.numSizes() == 0);
    }

    SECTION("Size 2 integer tensor") {
        IntTensor intTensor{(NumSizes){1}, 2};
        IntTensorR *intTensorR = &intTensor.getR();
        IntTensorRW *intTensorRW = &intTensor.getRW();

        SECTION("Integer tensor check size") {
            REQUIRE(intTensor.numSizes() == 1);
            REQUIRE(intTensor.sizes()[0] == 2);
            REQUIRE(intTensorR->numSizes == 1);
            REQUIRE(intTensorR->sizes[0] == 2);
            REQUIRE(intTensorRW->numSizes == 1);
            REQUIRE(intTensorRW->sizes[0] == 2);
        }

        SECTION("Integer tensor write & read") {
            int *rw = intTensorRW->contents;
            rw[0] = 5;
            rw[1] = 6;

            int const *r = intTensorR->contents;
            REQUIRE(r[0] == 5);
            REQUIRE(r[1] == 6);
        }
    }

    SECTION("Size (2, 3) integer tensor") {
        IntTensor intTensor{(NumSizes){2}, 2, 3};
        IntTensorR *intTensorR = &intTensor.getR();
        IntTensorRW *intTensorRW = &intTensor.getRW();

        SECTION("Integer tensor check size") {
            REQUIRE(intTensor.numSizes() == 2);
            REQUIRE(intTensor.sizes()[0] == 2);
            REQUIRE(intTensor.sizes()[1] == 3);
            REQUIRE(intTensorR->numSizes == 2);
            REQUIRE(intTensorR->sizes[0] == 2);
            REQUIRE(intTensorR->sizes[1] == 3);
            REQUIRE(intTensorRW->numSizes == 2);
            REQUIRE(intTensorRW->sizes[0] == 2);
            REQUIRE(intTensorRW->sizes[1] == 3);
        }

        SECTION("Integer tensor write & read") {
            int const *s = intTensorRW->sizes;
            int *rw = intTensorRW->contents;
            rw[INDEX(s, 0, 0)] = 0;
            rw[INDEX(s, 0, 1)] = 1;
            rw[INDEX(s, 0, 2)] = 2;
            rw[INDEX(s, 1, 0)] = 3;
            rw[INDEX(s, 1, 1)] = 4;
            rw[INDEX(s, 1, 2)] = 5;

            int const *r = intTensorR->contents;
            for (int i = 0; i < 6; i++) {
                REQUIRE(r[i] == i);
            }
        }
    }
}

TEST_CASE("Create float tensor", "[data_interface]") {
    SECTION("Default construction") {
        FloatTensor floatTensor{};
        REQUIRE(floatTensor.numSizes() == 0);
    }

    SECTION("Size (2, 2, 2) float tensor") {
        FloatTensor floatTensor{(NumSizes){3}, 2, 2, 2};
        FloatTensorR *floatTensorR = &floatTensor.getR();
        FloatTensorRW *floatTensorRW = &floatTensor.getRW();

        SECTION("Float tensor check size") {
            REQUIRE(floatTensor.numSizes() == 3);
            REQUIRE(floatTensor.sizes()[0] == 2);
            REQUIRE(floatTensor.sizes()[1] == 2);
            REQUIRE(floatTensor.sizes()[2] == 2);
            REQUIRE(floatTensorR->numSizes == 3);
            REQUIRE(floatTensorR->sizes[0] == 2);
            REQUIRE(floatTensorR->sizes[1] == 2);
            REQUIRE(floatTensorR->sizes[2] == 2);
            REQUIRE(floatTensorRW->numSizes == 3);
            REQUIRE(floatTensorRW->sizes[0] == 2);
            REQUIRE(floatTensorRW->sizes[1] == 2);
            REQUIRE(floatTensorRW->sizes[2] == 2);
        }

        SECTION("Float tensor write & read") {
            int const *s = floatTensorRW->sizes;
            float *rw = floatTensorRW->contents;
            rw[INDEX(s, 0, 0, 0)] = 0.0f;
            rw[INDEX(s, 0, 0, 1)] = 0.1f;
            rw[INDEX(s, 0, 1, 0)] = 0.2f;
            rw[INDEX(s, 0, 1, 1)] = 0.3f;
            rw[INDEX(s, 1, 0, 0)] = 0.4f;
            rw[INDEX(s, 1, 0, 1)] = 0.5f;
            rw[INDEX(s, 1, 1, 0)] = 0.6f;
            rw[INDEX(s, 1, 1, 1)] = 0.7f;

            float const *r = floatTensorR->contents;
            for (int i = 0; i < 8; i++) {
                REQUIRE(r[i] == (float) i / 10);
            }
        }
    }
}

TEST_CASE("Create branches", "[data_interface]") {
    SECTION("Empty branch") {
        Branch empty{0};
        BranchR *branchR = &empty;
        BranchRW *branchRW = &empty;

        REQUIRE(empty.size() == 0);
        REQUIRE(branchR_getSize(branchR) == 0);
        REQUIRE(branchRW_getSize(branchRW) == 0);
    }

    SECTION("Int / Float tuple as a branch") {
        Branch branch{2};
        BranchR *branchR;
        BranchRW *branchRW;

        {
            std::unique_ptr<IntTensor> intScalar;
            std::unique_ptr<FloatTensor> floatScalar;

            intScalar = make_unique<IntTensor>((NumSizes){1}, 1);
            floatScalar = make_unique<FloatTensor>((NumSizes){1}, 1);

            intScalar->contents()[0] = 5;
            floatScalar->contents()[0] = 6.0f;

            branch.setValue(0, std::move(intScalar));
            branch.setValue(1, std::move(floatScalar));

            branchR = &branch;
            branchRW = &branch;
        }

        SECTION("Accessing via BranchR") {
            IntTensorR *intScalar;
            FloatTensorR *floatScalar;

            intScalar = branchR_getIntTensor(branchR, 0);
            floatScalar = branchR_getFloatTensor(branchR, 1);

            REQUIRE(intScalar->contents[0] == 5);
            REQUIRE(floatScalar->contents[0] == 6.0f);
        }
        
        SECTION("Accessing via BranchRW") {
            IntTensorRW *intScalar;
            FloatTensorRW *floatScalar;

            intScalar = branchRW_getIntTensor(branchRW, 0);
            floatScalar = branchRW_getFloatTensor(branchRW, 1);

            intScalar->contents[0] = 4;
            floatScalar->contents[0] = 5.0f;

            REQUIRE(intScalar->contents[0] == 4);
            REQUIRE(floatScalar->contents[0] == 5.0f);
        }
    }

    SECTION("Branch containing empty branch") {
        Branch branch{2};
        BranchR *branchR;
        BranchRW *branchRW;

        {
            std::unique_ptr<Branch> branchEmpty = make_unique<Branch>(0);

            branch.setValue(0, std::move(branchEmpty));
            branchR = &branch;
            branchRW = &branch;
        }

        SECTION("Accessing via BranchR") {
            BranchR *branchEmpty = branchR_getBranch(branchR, 0);
            REQUIRE(branchR_getSize(branchEmpty) == 0);
        }

        SECTION("Accessing via BranchRW") {
            BranchRW *branchEmpty = branchRW_getBranch(branchRW, 0);
            REQUIRE(branchRW_getSize(branchEmpty) == 0);
        }
    }

    SECTION("Modifying branches") {
        Branch branch{1};
        BranchRW *branchRW = &branch;

        IntTensorRW *intTensor;
        FloatTensorRW *floatTensor;
        BranchRW *branchInner;

        intTensor = branchRW_makeIntTensor(branchRW, 0, (NumSizes){1}, 1);
        REQUIRE(intTensor->numSizes == 1);

        floatTensor = branchRW_makeFloatTensor(branchRW, 0, (NumSizes){1}, 1);
        REQUIRE(floatTensor->numSizes == 1);

        branchInner = branchRW_makeBranch(branchRW, 0, 1);
        REQUIRE(branchRW_getSize(branchInner) == 1);

        intTensor = branchRW_makeIntTensor(
            branchRW, APPEND_INDEX, (NumSizes){2}, 1, 1);
        REQUIRE(intTensor->numSizes == 2);
        REQUIRE(branchRW_getSize(branchRW) == 2);
        REQUIRE(branchRW_getIntTensor(branchRW, 1) == intTensor);

        floatTensor = branchRW_makeFloatTensor(
            branchRW, APPEND_INDEX, (NumSizes){2}, 1, 1);
        REQUIRE(floatTensor->numSizes == 2);
        REQUIRE(branchRW_getSize(branchRW) == 3);
        REQUIRE(branchRW_getFloatTensor(branchRW, 2) == floatTensor);

        branchInner = branchRW_makeBranch(
            branchRW, APPEND_INDEX, 2);
        REQUIRE(branchRW_getSize(branchInner) == 2);
        REQUIRE(branchRW_getSize(branchRW) == 4);
        REQUIRE(branchRW_getBranch(branchRW, 3) == branchInner);

        branchRW_pop(branchRW);
        REQUIRE(branchRW_getSize(branchRW) == 3);
    }
}

TEST_CASE("Data block basic access", "[data_interface]") {
    DataBlock blocks[3]{{1, 0}, {1, 0}, {1, 0}};

    {
        std::unique_ptr<IntTensor> intTensor;
        std::unique_ptr<FloatTensor> floatTensor;
        std::unique_ptr<Branch> branch;

        intTensor = make_unique<IntTensor>((NumSizes){2}, 1, 1);
        intTensor->contents()[0] = 5;

        floatTensor = make_unique<FloatTensor>((NumSizes){3}, 1, 1, 1);
        floatTensor->contents()[0] = 6.0f;

        branch = make_unique<Branch>(0);

        blocks[0].setInputMsg(std::move(intTensor));
        blocks[1].setInputMsg(std::move(floatTensor));
        blocks[2].setInputMsg(std::move(branch));
    }

    SECTION("Access data block input values") {
        IntTensorR *intTensor;
        FloatTensorR *floatTensor;
        BranchR *branch;

        intTensor = inputMsg_getIntTensor(&blocks[0]);
        REQUIRE(intTensor->numSizes == 2);
        REQUIRE(intTensor->sizes[0] == 1);
        REQUIRE(intTensor->contents[0] == 5);

        floatTensor = inputMsg_getFloatTensor(&blocks[1]);
        REQUIRE(floatTensor->contents[0] == 6.0f);

        branch = inputMsg_getBranch(&blocks[2]);
        REQUIRE(branchR_getSize(branch) == 0);
    }

    SECTION("Create data block output values") {
        IntTensorRW *intTensor;
        FloatTensorRW *floatTensor;
        BranchRW *branch;

        intTensor = outputMsg_makeIntTensor(
            &blocks[0], 0, (NumSizes){1}, 1);
        REQUIRE(intTensor->numSizes == 1);

        floatTensor = outputMsg_makeFloatTensor(
            &blocks[1], 0, (NumSizes){1}, 1);
        REQUIRE(floatTensor->numSizes == 1);

        branch = outputMsg_makeBranch(&blocks[2], 0, 1);
        REQUIRE(branchRW_getSize(branch) == 1);

        intTensor = outputMsg_moveIntTensor(&blocks[0], 0);
        REQUIRE(intTensor->numSizes == 2);

        floatTensor = outputMsg_moveFloatTensor(&blocks[1], 0);
        REQUIRE(floatTensor->numSizes == 3);

        branch = outputMsg_moveBranch(&blocks[2], 0);
        REQUIRE(branchRW_getSize(branch) == 0);
    }
}

TEST_CASE("Data block continuous data access", "[data_interface]") {
    DataBlock block{0, 1, 1};

    std::unique_ptr<DataReference> inDataRef = make_unique<DataReference>();
    std::unique_ptr<DataReference> outDataRef = make_unique<DataReference>();
    block.setInputContData(0, inDataRef.get());
    REQUIRE(block.getInputContData(0) == inDataRef.get());
    block.setOutputContData(0, outDataRef.get());
    REQUIRE(block.getOutputContData(0) == outDataRef.get());

    IntTensorRW *intTensor;
    FloatTensorRW *floatTensor;
    BranchRW *branch;
    DataSharedPtr readback;

    SECTION("Data block reading continuous data") {
        std::unique_ptr<IntTensor> intTensor =
            make_unique<IntTensor>((NumSizes){1}, 1);
        intTensor->contents()[0] = 3;
        inDataRef->setWriteAcquire(std::move(intTensor));
        inDataRef->writeFinalize();
        REQUIRE(inputContData_getIntTensor(&block, 0)->contents[0] == 3);

        std::unique_ptr<FloatTensor> floatTensor =
            make_unique<FloatTensor>((NumSizes){1}, 1);
        floatTensor->contents()[0] = 4.0f;
        inDataRef->setWriteAcquire(std::move(floatTensor));
        inDataRef->writeFinalize();
        REQUIRE(inputContData_getFloatTensor(&block, 0)->contents[0] == 4.0f);

        std::unique_ptr<Branch> branch = make_unique<Branch>(0);
        inDataRef->setWriteAcquire(std::move(branch));
        inDataRef->writeFinalize();
        REQUIRE(branchR_getSize(inputContData_getBranch(&block, 0)) == 0);
    }

    SECTION("Data block writing continuous data") {
        intTensor = outputContData_writeIntTensor(&block, 0, (NumSizes){1}, 1);
        intTensor->contents[0] = 3;
        block.finalize();
        readback = block.getOutputContData(0)->readAcquire();
        REQUIRE(dynamic_cast<IntTensor *>(readback.get())
            ->contents()[0] == 3);

        intTensor = outputContData_writeIntTensor(&block, 0, (NumSizes){1}, 1);
        intTensor->contents[0] = 4;
        block.finalize();
        readback = block.getOutputContData(0)->readAcquire();
        REQUIRE(dynamic_cast<IntTensor *>(readback.get())
            ->contents()[0] == 4);

        outDataRef = make_unique<DataReference>();
        block.setOutputContData(0, outDataRef.get());
        floatTensor = outputContData_writeFloatTensor(&block, 0, (NumSizes){1}, 1);
        floatTensor->contents[0] = 4.0f;
        block.finalize();
        readback = block.getOutputContData(0)->readAcquire();
        REQUIRE(dynamic_cast<FloatTensor *>(readback.get())
            ->contents()[0] == 4.0f);

        outDataRef = make_unique<DataReference>();
        block.setOutputContData(0, outDataRef.get());
        branch = outputContData_writeBranch(&block, 0, 0);
        block.finalize();
        readback = block.getOutputContData(0)->readAcquire();
        REQUIRE(dynamic_cast<Branch *>(readback.get()) == branch);
    }

    SECTION("Data block setting continuous data") {
        intTensor = outputContData_setIntTensor(&block, 0, (NumSizes){1}, 1);
        intTensor->contents[0] = 3;
        block.finalize();
        readback = block.getOutputContData(0)->readAcquire();
        REQUIRE(dynamic_cast<IntTensor *>(readback.get())
            ->contents()[0] == 3);

        intTensor = outputContData_setIntTensor(&block, 0, (NumSizes){1}, 2);
        block.finalize();
        REQUIRE(intTensor->sizes[0] == 2);

        floatTensor = outputContData_setFloatTensor(&block, 0, (NumSizes){1}, 1);
        floatTensor->contents[0] = 4.0f;
        block.finalize();
        readback = block.getOutputContData(0)->readAcquire();
        REQUIRE(dynamic_cast<FloatTensor *>(readback.get())
            ->contents()[0] == 4.0f);

        branch = outputContData_setBranch(&block, 0, 0);
        block.finalize();
        readback = block.getOutputContData(0)->readAcquire();
        REQUIRE(dynamic_cast<Branch *>(readback.get()) == branch);
    }

    SECTION("Data block finalizing continuous data") {
        REQUIRE(block.getOutputContData(0)->readAcquire() == nullptr);

        outputContData_writeBranch(&block, 0, 0);

        REQUIRE(block.getOutputContData(0)->readAcquire() == nullptr);

        block.finalize();

        REQUIRE(block.getOutputContData(0)->readAcquire() != nullptr);
    }
}