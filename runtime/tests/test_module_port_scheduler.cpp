#include <string>
#include <thread>
#include <custom_utility.hpp>

#include "catch.hpp"
#include "threaded_test_util.hpp"
#include "module_port_scheduler.hpp"

TEST_CASE("Message queue functionality") {
    MessageQueue queue{2};
    DataPtr intTensors[3];

    for (int i = 0; i < 3; i++) {
        std::unique_ptr<IntTensor> result =
            make_unique<IntTensor>(NumSizes{1}, 1);
        result->contents()[0] = i;
        intTensors[i] = std::move(result);
    }

    SECTION("Not reaching capacity") {
        REQUIRE(queue.size() == 0);
        queue.enqueue(std::move(intTensors[0]));
        queue.enqueue(std::move(intTensors[1]));
        REQUIRE(queue.size() == 2);
        intTensors[0] = queue.dequeue();
        REQUIRE(queue.size() == 1);
        REQUIRE(static_cast<IntTensor *>(
            intTensors[0].get())->contents()[0] == 0);
    }

    SECTION("Reaching capacity") {
        queue.enqueue(std::move(intTensors[0]));
        queue.enqueue(std::move(intTensors[1]));
        queue.enqueue(std::move(intTensors[2]));
        REQUIRE(queue.size() == 2);
        intTensors[1] = queue.dequeue();
        REQUIRE(static_cast<IntTensor *>(
            intTensors[1].get())->contents()[0] == 1);
    }
}

TEST_CASE("Basic properties of the scheduler") {
    ModulePortScheduler scheduler{3, 2};

    SECTION("Initial setup") {
        REQUIRE(scheduler.numDataReady() == 0);
        REQUIRE(scheduler.numModulePortsReady() == 3);
        REQUIRE(scheduler.numModulePortsPending() == 0);
    }

    SECTION("Values update based on data and module readiness") {
        scheduler.setDataReady(0, true);
        REQUIRE(scheduler.numDataReady() == 1);
        REQUIRE(scheduler.numModulePortsReady() == 3);
        REQUIRE(scheduler.numModulePortsPending() == 1);

        scheduler.setModulePortReady(1, false);
        REQUIRE(scheduler.numDataReady() == 1);
        REQUIRE(scheduler.numModulePortsReady() == 2);
        REQUIRE(scheduler.numModulePortsPending() == 1);

        scheduler.setModulePortReady(0, false);
        REQUIRE(scheduler.numDataReady() == 1);
        REQUIRE(scheduler.numModulePortsReady() == 1);
        REQUIRE(scheduler.numModulePortsPending() == 0);

        scheduler.setModulePortReady(1, true);
        REQUIRE(scheduler.numDataReady() == 1);
        REQUIRE(scheduler.numModulePortsReady() == 2);
        REQUIRE(scheduler.numModulePortsPending() == 0);

        scheduler.setModulePortReady(0, true);
        REQUIRE(scheduler.numDataReady() == 1);
        REQUIRE(scheduler.numModulePortsReady() == 3);
        REQUIRE(scheduler.numModulePortsPending() == 1);

        scheduler.setDataReady(0, false);
        REQUIRE(scheduler.numDataReady() == 0);
        REQUIRE(scheduler.numModulePortsReady() == 3);
        REQUIRE(scheduler.numModulePortsPending() == 0);
    }

    SECTION("Next module identified correctly") {
        scheduler.setDataReady(0, true);
        REQUIRE(scheduler.nextModulePort() == 0);

        scheduler.setDataReady(0, false);
        scheduler.setDataReady(1, true);
        REQUIRE(scheduler.nextModulePort() == 1);
    }

    SECTION("Next module accessed in round robin fashion") {
        scheduler.setDataReady(0, true);
        scheduler.setDataReady(1, true);
        REQUIRE(scheduler.nextModulePort() == 0);
        REQUIRE(scheduler.nextModulePort() == 1);
        REQUIRE(scheduler.nextModulePort() == 0);
        REQUIRE(scheduler.nextModulePort() == 1);

        scheduler.setModulePortReady(0, false);
        REQUIRE(scheduler.nextModulePort() == 1);
        REQUIRE(scheduler.nextModulePort() == 1);
    }

    SECTION("Contains message queues") {
        {
            MessageQueue &queue = scheduler.getQueue(0);
            queue.enqueue(DataPtr{});
            REQUIRE(queue.size() == 1);
        }{
            MessageQueue &queue = scheduler.getQueue(1);
            REQUIRE(queue.size() == 0);
        }
    }

    SECTION("Can be locked") {
        BooleanFlag threadActive{false};
        BooleanFlag threadGotLock{false};

        LockHandle schedulerLock = scheduler.lock();
        std::thread thread{[&] {
            threadActive.setAndSignal(true);
            
            LockHandle schedulerLock = scheduler.lock();
            
            threadGotLock.setAndSignal(true);
        }};
        thread.detach();

        threadActive.waitForValue(true);
        REQUIRE(!threadGotLock.value());
        schedulerLock.unlock();
        threadGotLock.waitForValue(true);
    }
}