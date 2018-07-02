#include "module_port_scheduler.hpp"

ModulePortScheduler::ModulePortScheduler(int numModules)
        :numDataReady_(0),
         numModulePortsReady_(numModules),
         numModulePortsPending_(0),
         dataReady_(numModules, false),
         modulePortsReady_(numModules, true),
         modulePortsPending_(numModules, false),
         pendingList_(0),
         pendingListIterators_(numModules, pendingList_.begin()) {}

void ModulePortScheduler::updateModulePortsPending(int modulePort) {
    bool wasPending = modulePortsPending_[modulePort];
    bool nowPending = (
        dataReady_[modulePort] && modulePortsReady_[modulePort]);

    if (nowPending && !wasPending) {
        modulePortsPending_[modulePort] = true;
        numModulePortsPending_ += 1;
        pendingList_.emplace_back(modulePort);
        pendingListIterators_[modulePort] = --pendingList_.end();
    } else if (!nowPending && wasPending) {
        modulePortsPending_[modulePort] = false;
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
    if (modulePortsReady_[modulePort] != value) {
        modulePortsReady_[modulePort] = value;
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