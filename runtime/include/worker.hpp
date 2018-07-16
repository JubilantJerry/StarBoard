#ifndef WORKER_HPP
#define WORKER_HPP

#include "data_interface.hpp"
#include "data_serializer.hpp"
#include "module_port_scheduler.hpp"
#include "module.hpp"

class Worker {
private:
    ModulePortScheduler scheduler_;
    std::vector<Module> modules_;
};

#endif