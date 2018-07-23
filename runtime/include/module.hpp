#ifndef MODULE_HPP
#define MODULE_HPP

#include <memory>

#include "data_interface.hpp"
#include "module_port_scheduler.hpp"
#include "module_data_handler.hpp"

using ModuleDataHandlerPtr = std::unique_ptr<ModuleDataHandler>;

class Module final {
private:
    ModulePortScheduler *scheduler_;

    int offset_;
    int numInputs_;
    std::vector<int> outModulePorts_;

    Module(ModulePortScheduler *scheduler,
           int offset,
           int numInputs,
           std::vector<int> &&outModulePorts);

    void testInputRange(int modulePort);

    friend class ModuleBuilder;

public:
    Module() {};

    int offset() const noexcept {
        return offset_;
    }

    int numInputs() const noexcept {
        return numInputs_;
    }

    DataBlock acquire(int modulePort, LockHandle lock);
    void release(int modulePort, DataBlock dataBlock);
};

class ModuleBuilder final {
private:
    int offset_;
    int numInputs_;
    std::vector<int> outModulePorts_;
public:
    ModuleBuilder & setOffset(int offset) {
        offset_ = offset;
        return *this;
    }

    ModuleBuilder & addInputMessagePort() {
        numInputs_ += 1;
        return *this;
    }

    ModuleBuilder & addOutputMessagePort(int outModulePort) {
        outModulePorts_.push_back(outModulePort);
        return *this;
    }

    Module build(ModulePortScheduler &scheduler) {
        return Module{
            &scheduler,
            offset_,
            numInputs_,
            std::move(outModulePorts_)
        };
    }
};

#endif