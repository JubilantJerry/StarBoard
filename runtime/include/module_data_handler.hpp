#ifndef MODULE_DATA_HANDLER_HPP
#define MODULE_DATA_HANDLER_HPP

#include "data_interface.hpp"
#include "native_loader.hpp"

class Module;

class ModuleDataHandler {
private:
    Module *module_;
    int modulePort_;

protected:
    Module *module() {
        return module_;
    }

    int modulePort() {
        return modulePort_;
    }

public:
    void linkToModule(Module *module, int modulePort) {
        module_ = module;
        modulePort_ = modulePort;
    }

    virtual void receiveData(DataBlock &&block) = 0;
};

class NativeModuleDataHandler: public ModuleDataHandler {
private:
    ModuleFunction native_handler_;

public:
    NativeModuleDataHandler(ModuleFunction native_handler)
            :native_handler_(native_handler) {}

    virtual void receiveData(DataBlock &&block) override;     
};

#endif