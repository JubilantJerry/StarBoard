#include <sstream>
#include <memory>
#include <utility>
#include <custom_utility.hpp>

#include "catch.hpp"
#include "data_interface.hpp"
#include "module.hpp"
#include "module_data_handler.hpp"


class LoggingDataHandler: public ModuleDataHandler {
private:
    std::string tag_;
    std::stringstream &log_;

public:
    LoggingDataHandler(std::string tag, std::stringstream &log)
            : tag_(tag), log_(log) {}

    void receiveData(DataBlock &&block) {
        log_ << "Port " << tag_ << " received data: " <<
            outputMsg_moveIntTensor(&block, 0)->contents[0] << "\n";
        module()->release(modulePort(), std::move(block));
    }
};

TEST_CASE("Module basic accessors") {
    ModulePortScheduler scheduler{3, 5};
    std::stringstream log;
    ModuleDataHandlerPtr moduleHandler =
        make_unique<LoggingDataHandler>("A", log);

    ModuleBuilder moduleBuilder;
    moduleBuilder
        .setOffset(2)
        .addInputMessagePort(std::move(moduleHandler));

    Module module = moduleBuilder.build(scheduler);

    REQUIRE(module.offset() == 2);
    REQUIRE(module.numInputs() == 1);
}

TEST_CASE("Module correctly offloading functionality") {
    ModulePortScheduler scheduler{3, 5};
    std::stringstream log;
    ModuleDataHandlerPtr moduleHandlers[2] = {
        make_unique<LoggingDataHandler>("A", log),
        make_unique<LoggingDataHandler>("B", log)
    };

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
        ModuleBuilder moduleBuilder;
        moduleBuilder
            .setOffset(1)
            .addInputMessagePort(std::move(moduleHandlers[0]))
            .addInputMessagePort(std::move(moduleHandlers[1]))
            .addOutputMessagePort(0);

        Module module = moduleBuilder.build(scheduler);
        LockHandle schedulerLock;

        schedulerLock = scheduler.lock();
        REQUIRE(scheduler.modulePortPending(1));
        module.acquire(1, std::move(schedulerLock));
        REQUIRE(!scheduler.dataReady(1));
        REQUIRE(scheduler.modulePortReady(1));

        schedulerLock = scheduler.lock();
        REQUIRE(scheduler.modulePortPending(2));
        module.acquire(2, std::move(schedulerLock));
        REQUIRE(!scheduler.dataReady(2));
        REQUIRE(scheduler.modulePortReady(2));

        REQUIRE(scheduler.getQueue(0).size() == 2);
        REQUIRE(scheduler.dataReady(0));
        REQUIRE(log.str() == "Port A received data: 7\n"
                             "Port B received data: 13\n");
    }
    

}