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
    ModuleFunction addOne = loader.load(libDir, "add_one", "addOne");
    
    Module module = ModuleBuilder{}
        .setOffset(0)
        .addInputMessagePort()
        .addOutputMessagePort(1)
        .build(scheduler);

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
    }, [&] {
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