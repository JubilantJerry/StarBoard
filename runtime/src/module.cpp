#include <string>

#include "module.hpp"

Module::Module(ModulePortScheduler &scheduler,
               int offset,
               int numInputs,
               std::vector<int> &&outModulePorts)
    : scheduler_(scheduler),
      offset_(offset),
      numInputs_(numInputs),
      outModulePorts_(std::move(outModulePorts)) {}

inline void Module::testInputRange(int modulePort) {
    if (modulePort >= offset_ + numInputs_) {
        throw std::out_of_range(
            "Module port out of range: " + std::to_string(modulePort));
    }
}

DataBlock Module::acquire(int modulePort, LockHandle lock) {
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
    return dataBlock;
}

void Module::release(int modulePort, DataBlock dataBlock) {
    (void)modulePort;

    IntSet const &outputMsgWritten = dataBlock.getOutputMsgWritten();
    IntSet::const_iterator end = outputMsgWritten.end();

    for (IntSet::const_iterator it = outputMsgWritten.begin();
         it != end; it++) {

        int outPortNum = *it;
        DataPtr data = dataBlock.takeOutputMsg(outPortNum);
        int outModulePort = outModulePorts_[outPortNum];
        int queueNumber = scheduler_.pendingQueueAssignment(outModulePort);

        LockHandle lock = scheduler_.lock(queueNumber);

        scheduler_.getQueue(outModulePort).enqueue(std::move(data));
        scheduler_.setDataReady(outModulePort, true);
    }
}