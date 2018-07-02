#ifndef MODULE_PORT_SCHEDULER_H
#define MODULE_PORT_SCHEDULER_H

#include <vector>
#include <list>
#include <boost/pool/pool_alloc.hpp>

class ModulePortScheduler {
private:
    int numDataReady_;
    int numModulePortsReady_;
    int numModulePortsPending_;

    std::vector<int> dataReady_;
    std::vector<int> modulePortsReady_;
    std::vector<int> modulePortsPending_;

    std::list<int, boost::fast_pool_allocator<int>> pendingList_;
    std::vector<std::list<int>::iterator> pendingListIterators_;

    void updateModulePortsPending(int modulePort);

public:
    ModulePortScheduler(int numModules);

    int numDataReady() noexcept {
        return numDataReady_;
    }

    int numModulePortsReady() noexcept {
        return numModulePortsReady_;
    }

    int numModulePortsPending() noexcept {
        return numModulePortsPending_;
    }

    void setDataReady(int modulePort, bool value);

    void setModulePortReady(int modulePort, bool value);

    int nextModulePort();
};

#endif