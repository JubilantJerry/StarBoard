#include <string>
#include "module.hpp"

Module::Module(ModulePortScheduler &scheduler,
       int offset,
       std::vector<ModuleDataHandlerPtr> &&dataHandlers,
       std::vector<int> &&outModulePorts)

    : scheduler_(scheduler),
      offset_(offset),
      dataHandlers_(std::move(dataHandlers)),
      outModulePorts_(std::move(outModulePorts)) {

    int numInputs = dataHandlers_.size();
    for (int i = 0; i < numInputs; i++) {
        dataHandlers_[i]->linkToModule(this, offset_ + i);
    }
}

inline void Module::testInputRange(int modulePort) {
    int numInputs = dataHandlers_.size();
    if (modulePort >= offset_ + numInputs) {
        throw std::out_of_range(
            "Module port out of range: " + std::to_string(modulePort));
    }
}

void Module::acquire(int modulePort, LockHandle lock) {
    testInputRange(modulePort);
    MessageQueue &queue = scheduler_.getQueue(modulePort);
    DataPtr data = queue.dequeue();
    if (queue.size() == 0) {
        scheduler_.setDataReady(modulePort, false);
    }
    lock.unlock();

    int numOutputs = outModulePorts_.size();
    DataBlock dataBlock{numOutputs};
    dataBlock.setInputMsg(std::move(data));
    dataHandlers_[modulePort - offset_]->receiveData(std::move(dataBlock));
}

void Module::release(int modulePort, DataBlock dataBlock) {
    (void)modulePort;

    int numOutputs = dataBlock.numOutputs();

    LockHandle lock = scheduler_.lock();
    for (int i = 0; i < numOutputs; i++) {
        DataPtr data = dataBlock.takeOutputMsg(i);
        if (data) {
            int outModulePort = outModulePorts_[i];
            scheduler_.getQueue(outModulePort).enqueue(std::move(data));
            scheduler_.setDataReady(outModulePort, true);
        }
    }
}