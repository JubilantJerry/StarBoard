#include <thread>
#include <unistd.h>

#include "worker.hpp"

Worker::Worker(ModulePortScheduler &scheduler,
               std::vector<Module> &&modules,
               std::vector<ModuleFunction> &&moduleDataHandlers,
               GlobalProperties &&properties)
    : scheduler_(scheduler),
      modules_(std::move(modules)),
      moduleDataHandlers_(moduleDataHandlers),
      properties_(std::move(properties)),
      worldCommunicator_(
        properties_.workerName, scheduler_, WORLD_QUEUE_NUM) {}

void Worker::workTask(
        ModulePortScheduler &scheduler,
        std::vector<Module> &modules,
        std::vector<ModuleFunction> &moduleDataHandlers) {

    std::vector<int> modulePortToModule(scheduler.numModulePorts());

    int numModules = modules.size();

    for (int moduleNum = 0; moduleNum < numModules; moduleNum++) {
        Module const &module = modules[moduleNum];
        int offset = module.offset();
        int numInputs = module.numInputs();

        for (int modulePort = offset;
             modulePort < offset + numInputs; modulePort++) {

            modulePortToModule[modulePort] = moduleNum;
        }
    }

    while (true) {
        int modulePort;
        int moduleNum;
        DataBlock block;

        {
            LockHandle schedulerLock = scheduler.waitPending(WORK_QUEUE_NUM);
            modulePort = scheduler.nextModulePort(WORK_QUEUE_NUM);
            moduleNum = modulePortToModule[modulePort];
            block = modules[moduleNum].acquire(
                modulePort, std::move(schedulerLock));
        }

        {
            moduleDataHandlers[modulePort](&block);
            LockHandle schedulerLock = scheduler.lock(WORK_QUEUE_NUM);
            modules[moduleNum].release(modulePort, std::move(block));
        }

    }
}

void Worker::run() {
    std::thread worldReceiver{[&]() {
        worldCommunicator_.receiveTask();
    }};
    worldReceiver.detach();

    std::thread worldSender{[&]() {
        worldCommunicator_.sendTask();
    }};
    worldSender.detach();

    std::thread process{[&]() {
        Worker::workTask(scheduler_, modules_, moduleDataHandlers_);
    }};
    process.detach();

    while (true) {
        pause();
    }
}
