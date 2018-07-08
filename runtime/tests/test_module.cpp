#include <sstream>
#include <memory>
#include <utility>
#include <custom_utility.hpp>

#include "catch.hpp"
#include "native_interface.hpp"
#include "module.hpp"


class LoggingDataHandler: public ModuleDataHandler {
private:
    std::string tag_;
    std::stringstream &log_;

    Module *module_;
    int modulePortNum_;

public:
    LoggingDataHandler(std::string tag, std::stringstream &log)
            : tag_(tag), log_(log) {}

    void linkToModule(Module *module, int modulePortNum) {
        module_ = module;
        modulePortNum_ = modulePortNum;
    }

    void receiveData(DataBlock &&block) {
        log_ << "Port " << tag_ << " received data: " <<
            outputMsg_moveIntTensor(&block, 0)->contents[0] << "\n";
        module_->release(modulePortNum_, std::move(block));
    }
};

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
    scheduler.getQueue(2).enqueue(std::move(intTensors[1]));

    SECTION("Two port module with no resource requirements") {
        ModuleBuilder moduleBuilder;
        moduleBuilder.setOffset(1);
        moduleBuilder.addInputMessagePort(std::move(moduleHandlers[0]));
        moduleBuilder.addInputMessagePort(std::move(moduleHandlers[1]));
        moduleBuilder.addOutputMessagePort(0);

        Module module = moduleBuilder.build(scheduler);

        REQUIRE(scheduler.getQueue(0).size() == 0);
        LockHandle schedulerLock;

        schedulerLock = scheduler.lock();
        REQUIRE(scheduler.modulePortReady(1));
        module.acquire(1, std::move(schedulerLock));

        schedulerLock = scheduler.lock();
        REQUIRE(scheduler.modulePortReady(2));
        module.acquire(2, std::move(schedulerLock));

        REQUIRE(scheduler.getQueue(0).size() == 2);
        REQUIRE(log.str() == "Port A received data: 7\n"
                             "Port B received data: 13\n");
    }
    

}