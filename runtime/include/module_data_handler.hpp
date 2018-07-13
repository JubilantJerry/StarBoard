#ifndef MODULE_DATA_HANDLER_HPP
#define MODULE_DATA_HANDLER_HPP

#include "data_interface.hpp"

class Module;

class ModuleDataHandler {
public:
    virtual void linkToModule(Module *module, int modulePort) = 0;

    virtual void receiveData(DataBlock &&block) = 0;
};

#endif