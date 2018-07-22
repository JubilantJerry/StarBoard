#ifndef MODULE_HPP
#define MODULE_HPP

#include <memory>

#include "data_interface.hpp"
#include "module_port_scheduler.hpp"

class ModuleDataHandler;

using ModuleDataHandlerPtr = std::unique_ptr<ModuleDataHandler>;

class Module final {
private:
    ModulePortScheduler &scheduler_;

    int offset_;
    std::vector<ModuleDataHandlerPtr> dataHandlers_;
    std::vector<int> outModulePorts_;

    Module(ModulePortScheduler &scheduler,
           int offset,
           std::vector<ModuleDataHandlerPtr> &&dataHandlers,
           std::vector<int> &&outModulePorts);

    void testInputRange(int modulePort);

    friend class ModuleBuilder;
public:
    int offset() {
        return offset_;
    }

    int numInputs() {
        return dataHandlers_.size();
    }

    void acquire(int modulePort, LockHandle lock);
    void release(int modulePort, DataBlock dataBlock);
};

class ModuleBuilder final {
private:
    int offset_;
    std::vector<ModuleDataHandlerPtr> dataHandlers_;
    std::vector<int> outModulePorts_;
public:
    ModuleBuilder & setOffset(int offset) {
        offset_ = offset;
        return *this;
    }

    ModuleBuilder & addInputMessagePort(
            ModuleDataHandlerPtr &&moduleDataHandler) {

        dataHandlers_.push_back(std::move(moduleDataHandler));
        return *this;
    }

    ModuleBuilder & addOutputMessagePort(int outModulePort) {
        outModulePorts_.push_back(outModulePort);
        return *this;
    }

    Module build(ModulePortScheduler &scheduler) {
        return Module{
            scheduler,
            offset_,
            std::move(dataHandlers_),
            std::move(outModulePorts_)
        };
    }
};

#endif