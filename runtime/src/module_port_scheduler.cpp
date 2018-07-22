#include "module_port_scheduler.hpp"

ModulePortScheduler::ModulePortScheduler(int numModules, int maxQueueSize)
        :numDataReady_(0),
         numModulePortsReady_(numModules),
         numModulePortsPending_(0),
         dataReady_(numModules, false),
         modulePortReady_(numModules, true),
         modulePortPending_(numModules, false),
         pendingList_(0),
         pendingListIterators_(numModules, pendingList_.begin()),
         queues_(numModules) {

    for (int i = 0; i < numModules; i++) {
        queues_[i] = std::move(MessageQueue{maxQueueSize});
    }
 }

void ModulePortScheduler::updateModulePortsPending(int modulePort) {
    bool wasPending = modulePortPending_[modulePort];
    bool nowPending = (
        dataReady_[modulePort] && modulePortReady_[modulePort]);

    if (nowPending && !wasPending) {
        if (numModulePortsPending_ == 0) {
            waitPendingCV_.notify_one();
        }
        modulePortPending_[modulePort] = true;
        numModulePortsPending_ += 1;
        pendingList_.emplace_back(modulePort);
        pendingListIterators_[modulePort] = --pendingList_.end();

    } else if (!nowPending && wasPending) {
        modulePortPending_[modulePort] = false;
        numModulePortsPending_ -= 1;
        pendingList_.erase(pendingListIterators_[modulePort]);
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

int ModulePortScheduler::nextModulePort() {
    int modulePort = pendingList_.front();
    pendingList_.pop_front();
    pendingList_.emplace_back(modulePort);
    pendingListIterators_[modulePort] = --pendingList_.end();
    return modulePort;
}