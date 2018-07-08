#ifndef MODULE_HPP
#define MODULE_HPP

#include <memory>

#include "native_interface.hpp"
#include "module_port_scheduler.hpp"

class Module;

class ModuleDataHandler {
public:
    virtual void linkToModule(Module *module, int modulePortNum) = 0;

    virtual void receiveData(DataBlock &&block) = 0;
};

using ModuleDataHandlerPtr = std::unique_ptr<ModuleDataHandler>;

class Module final {
private:
    friend class ModuleBuilder;
    Module() {}
public:
    void acquire(int modulePortNum, LockHandle &&lock) {
        LockHandle temp = std::move(lock);
    }
    void release(int modulePortNum, DataBlock &&dataBlock) {}
};

class ModuleBuilder final {
public:
    void setOffset(int offset) {}
    void addInputMessagePort(ModuleDataHandlerPtr &&moduleDataHandler) {}
    void addOutputMessagePort(int destModulePortNum) {}
    Module build(ModulePortScheduler &scheduler) {
        return Module{};
    }
};

#endif