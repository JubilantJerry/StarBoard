#include "module.hpp"
#include "module_data_handler.hpp"

void NativeModuleDataHandler::receiveData(DataBlock &&block) {
    native_handler_(&block);
    module()->release(modulePort(), std::move(block));
}