#include <thread>
#include <custom_utility.hpp>

#include "catch.hpp"
#include "catch_test_util.hpp"
#include "native_loader.hpp"
#include "module_port_scheduler.hpp"
#include "module.hpp"
#include "worker.hpp"
#include "stream_protocol.hpp"
#include "data.hpp"

#define LIB_DIR "build/lib/"

TEST_CASE("Minimal worker process") {
    std::string workerName = "starboard_test_worker";
    GlobalProperties properties{
        .maxQueueSize = 5, .workerName = workerName};

    ModulePortScheduler scheduler{2, 2, properties.maxQueueSize, {0, 1}};

    NativeLoader loader{};
    std::string libDir = buildDir + "lib/";
    
    Module module = ModuleBuilder{}
        .setOffset(0)
        .addInputMessagePort()
        .addOutputMessagePort(1)
        .build(scheduler);

    SECTION("Minimal worker add one") {
        ModuleFunction addOne = loader.load(libDir, "add_one", "addOne");

        Worker worker{
            scheduler,
            {module},
            {addOne, nullptr},
            std::move(properties)
        };

        forkChild(
        [&] {
            worker.run();
            return true;
        }, [&](int) {
            StreamHandlePtr worldStream = LocalStreamHandle::clientEnd(
                workerName + WORLD_ENDPOINT_SUFFIX);

            DataPtr data = make_unique<IntTensor>(NumSizes{1}, 1);
            IntTensor &intData =
                *dynamic_cast<IntTensor *>(data.get());
            intData.contents()[0] = 12;

            sendModulePort(*worldStream, 0);
            sendMessage(*worldStream, data);

            int receivedModulePort = receiveModulePort(*worldStream);
            DataPtr receivedData = receiveMessage(*worldStream);
            IntTensor &receivedIntData =
                *dynamic_cast<IntTensor *>(receivedData.get());

            return (receivedModulePort == 1) &&
                (receivedIntData.contents()[0] == intData.contents()[0] + 1);
        }, true);
    }

    SECTION("Minimal worker drops under high load") {
        ModuleFunction slowNop = loader.load(libDir, "slow_nop", "slowNop");

        Worker worker{
            scheduler,
            {module},
            {slowNop, nullptr},
            std::move(properties)
        };

        forkChild(
        [&] {
            worker.run();
            return true;
        }, [&](int) {
            StreamHandlePtr worldStream = LocalStreamHandle::clientEnd(
                workerName + WORLD_ENDPOINT_SUFFIX);

            DataPtr data = make_unique<IntTensor>(NumSizes{1}, 1);
            static_cast<IntTensor *>(data.get())->contents()[0] = 0;

            sendModulePort(*worldStream, 0);
            sendMessage(*worldStream, data);
            receiveModulePort(*worldStream);
            receiveMessage(*worldStream);

            for (int i = 0; i < 7; i++) {
                data = make_unique<IntTensor>(NumSizes{1}, 1);
                static_cast<IntTensor *>(data.get())->contents()[0] = i;
                sendModulePort(*worldStream, 0);
                sendMessage(*worldStream, data);
            }

            bool allCorrect = true;
            bool firstValue = true;
            for (int i = 0; i < 7; i++) {
                receiveModulePort(*worldStream);
                data = receiveMessage(*worldStream);
                int receivedValue = static_cast<IntTensor *>(
                    data.get())->contents()[0];

                if (firstValue && receivedValue == 0) {
                    // 0 was processed but 1 is skipped
                    i = 1;
                    firstValue = false;
                    continue;
                } else if (firstValue && receivedValue == 2) {
                    // 0 and 1 were skipped
                    i = 2;
                }
                allCorrect &= (receivedValue == i);
                firstValue = false;
            }
            return allCorrect;
        }, true);
    }
}