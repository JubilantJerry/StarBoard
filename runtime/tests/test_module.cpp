#include <memory>
#include <utility>
#include <custom_utility.hpp>

#include "catch.hpp"
#include "data_interface.hpp"
#include "module.hpp"

TEST_CASE("Module basic accessors") {
    ModulePortScheduler scheduler{3, 1, 5, {0, 0, 0}};

    Module module = ModuleBuilder{}
        .setOffset(2)
        .addInputMessagePort()
        .build(scheduler);

    REQUIRE(module.offset() == 2);
    REQUIRE(module.numInputs() == 1);
}

TEST_CASE("Module correctly offloading functionality") {
    ModulePortScheduler scheduler{3, 1, 5, {0, 0, 0}};

    DataPtr intTensors[2] = {
        make_unique<IntTensor>(NumSizes{1}, 1),
        make_unique<IntTensor>(NumSizes{1}, 1)
    };
    static_cast<IntTensor *>(intTensors[0].get())->contents()[0] = 7;
    static_cast<IntTensor *>(intTensors[1].get())->contents()[0] = 13;

    scheduler.getQueue(1).enqueue(std::move(intTensors[0]));
    scheduler.setDataReady(1, true);
    scheduler.getQueue(2).enqueue(std::move(intTensors[1]));
    scheduler.setDataReady(2, true);

    SECTION("Two port module with no resource requirements") {
        Module module = ModuleBuilder{}
            .setOffset(1)
            .addInputMessagePort()
            .addInputMessagePort()
            .addOutputMessagePort(0)
            .build(scheduler);

        {
            LockHandle schedulerLock = scheduler.lock(0);
            REQUIRE(scheduler.modulePortPending(1));
            DataBlock block = module.acquire(1, std::move(schedulerLock));

            REQUIRE(!scheduler.dataReady(1));
            REQUIRE(scheduler.modulePortReady(1));
            REQUIRE(inputMsg_getIntTensor(&block)->contents[0] == 7);

            outputMsg_makeIntTensor(&block, 0, NumSizes{1}, 1);
            module.release(1, std::move(block));
        }

        {
            LockHandle schedulerLock = scheduler.lock(0);
            REQUIRE(scheduler.modulePortPending(2));
            DataBlock block = module.acquire(2, std::move(schedulerLock));

            REQUIRE(!scheduler.dataReady(2));
            REQUIRE(scheduler.modulePortReady(2));
            REQUIRE(inputMsg_getIntTensor(&block)->contents[0] == 13);

            outputMsg_makeIntTensor(&block, 0, NumSizes{1}, 1);
            module.release(1, std::move(block));
        }

        REQUIRE(scheduler.getQueue(0).size() == 2);
        REQUIRE(scheduler.dataReady(0));
    }
    

}