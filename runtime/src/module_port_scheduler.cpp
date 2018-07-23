#include "module_port_scheduler.hpp"

ModulePortScheduler::ModulePortScheduler(
    int numModulePorts, int numPendingQueues, int maxQueueSize,
    std::vector<int> &&pendingQueueAssignment)
        :numDataReady_(0),
         numModulePortsReady_(numModulePorts),
         numModulePortsPending_(0),

         dataReady_(numModulePorts, false),
         modulePortReady_(numModulePorts, true),
         modulePortPending_(numModulePorts, false),

         pendingQueueAssignment_(std::move(pendingQueueAssignment)),
         pendingQueues_(numPendingQueues),
         pendingQueuePlaces_(numModulePorts, pendingQueues_[0].end()),

         queues_(numModulePorts),

         lockMutexes_(numPendingQueues),
         waitPendingCVs_(numPendingQueues) {

    for (int i = 0; i < numModulePorts; i++) {
        queues_[i] = std::move(MessageQueue{maxQueueSize});
    }
 }

void ModulePortScheduler::updateModulePortsPending(int modulePort) {
    bool wasPending = modulePortPending_[modulePort];
    bool nowPending = (
        dataReady_[modulePort] && modulePortReady_[modulePort]);

    int queueNumber = pendingQueueAssignment_[modulePort];

    if (nowPending && !wasPending) {
        SchedulingQueue &pendingQueue = pendingQueues_[queueNumber];

        if (pendingQueue.size() == 0) {
            waitPendingCVs_[queueNumber].notify_one();
        }
        modulePortPending_[modulePort] = true;
        numModulePortsPending_ += 1;
        SchedulingQueue::QueuePlace place = pendingQueue.enqueue(modulePort);
        pendingQueuePlaces_[modulePort] = place;

    } else if (!nowPending && wasPending) {
        modulePortPending_[modulePort] = false;
        numModulePortsPending_ -= 1;
        pendingQueues_[queueNumber].remove(pendingQueuePlaces_[modulePort]);
    }
}

void ModulePortScheduler::setDataReady(int modulePort, bool value) {
    if (dataReady_[modulePort] != value) {
        dataReady_[modulePort] = value;
        numDataReady_ += (value ? 1 : -1);
        updateModulePortsPending(modulePort);
    }
}

void ModulePortScheduler::setModulePortReady(int modulePort, bool value) {
    if (modulePortReady_[modulePort] != value) {
        modulePortReady_[modulePort] = value;
        numModulePortsReady_ += (value ? 1 : -1);
        updateModulePortsPending(modulePort);
    }
}

int ModulePortScheduler::nextModulePort(int queueNumber) {
    SchedulingQueue &pendingQueue = pendingQueues_[queueNumber];

    int modulePort = pendingQueue.dequeue();
    SchedulingQueue::QueuePlace place = pendingQueue.enqueue(modulePort);
    pendingQueuePlaces_[modulePort] = place;
    return modulePort;
}