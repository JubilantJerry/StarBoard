#ifndef WORKER_HPP
#define WORKER_HPP

#include <vector>

#include "module_port_scheduler.hpp"
#include "module.hpp"
#include "native_loader.hpp"
#include "communicator.hpp"

int constexpr WORK_QUEUE_NUM = 0;
int constexpr WORLD_QUEUE_NUM = 1;

struct GlobalProperties {
    int maxQueueSize;
    std::string workerName;
};

class Worker {
private:
    ModulePortScheduler &scheduler_;
    std::vector<Module> modules_;
    std::vector<ModuleFunction> moduleDataHandlers_;
    GlobalProperties properties_;

    WorldCommunicator worldCommunicator_;

    static void workTask(
            ModulePortScheduler &scheduler,
            std::vector<Module> &modules,
            std::vector<ModuleFunction> &moduleDataHandlers);

public:
    Worker(ModulePortScheduler &scheduler,
           std::vector<Module> &&modules,
           std::vector<ModuleFunction> &&moduleDataHandlers,
           GlobalProperties &&properties);

    void run();
};

#endif