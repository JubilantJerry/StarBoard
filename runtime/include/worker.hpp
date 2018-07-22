#ifndef WORKER_HPP
#define WORKER_HPP

#include <vector>
#include <utility>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include "data_interface.hpp"
#include "stream_protocol.hpp"
#include "stream_handle.hpp"
#include "module_port_scheduler.hpp"
#include "module.hpp"

char constexpr WORLD_ENDPOINT_SUFFIX[] = "-world";

struct GlobalProperties {
    int maxQueueSize;
    std::string workerName;
};

class Worker {
private:
    ModulePortScheduler scheduler_;
    std::vector<Module> modules_;
    std::vector<int> modulePortToModule_;
    std::string workerName_;

    static int constexpr WORLD_IN_MODULE_PORT = 0;

    static int constexpr WORLD_OUT_MODULE_PORT = 1;

    static void worldTask(
            ModulePortScheduler &scheduler,
            std::string const &workerName,
            int numInputs) {

        StreamHandlePtr worldStream;
        std::atomic<bool> streamKilled;

        std::mutex streamCreatedMutex;
        std::condition_variable streamCreatedCV;
        LockHandle streamCreatedLock{streamCreatedMutex};

        std::mutex outTaskDoneMutex;
        std::condition_variable outTaskDoneCV;
        LockHandle outTaskDoneLock{outTaskDoneMutex};

        std::thread inTask{[&] {
            while (true) {
                worldStream = LocalStreamHandle::serverEnd(
                    workerName + WORLD_ENDPOINT_SUFFIX);
                streamKilled.store(false);

                {
                    LockHandle lock{streamCreatedMutex};
                    streamCreatedCV.notify_all();
                }

                try {
                    while (true) {
                        StreamHandle &stream = *worldStream;
                        int modulePort = deserializeModulePortNum(stream);
                        DataPtr message = deserializeMessage(stream);

                        if (modulePort >= numInputs) {
                            throw StreamInvalidDataException(
                                "Input message port out of bounds");
                        }

                        LockHandle lock = scheduler.lock();

                        MessageQueue &queue = scheduler.getQueue(modulePort);
                        queue.enqueue(std::move(message));
                        scheduler.setDataReady(modulePort, true);
                    }
                } catch (StreamInterruptedException const &) {
                    // TODO: log the error somehow
                } catch (StreamInvalidDataException const &) {
                    // TODO: log the error somehow
                }

                streamKilled.store(true);
                outTaskDoneCV.wait(outTaskDoneLock);
            }
        }};
        inTask.detach();

        // outTask
        while (true) {
            streamCreatedCV.wait(streamCreatedLock);

            while (!streamKilled.load()) {
                LockHandle lock = scheduler.lock();

                MessageQueue &queue = scheduler.getQueue(modulePort);
                queue.enqueue(std::move(message));
                scheduler.setDataReady(modulePort, true);
            }

            {
                LockHandle lock{outTaskDoneMutex};
                outTaskDoneCV.notify_all();
            }
        }
        
    }

    static void worldOutTask(
            ModulePortScheduler &scheduler,
            StreamHandlePtr &worldStream,
            std::string const &workerName,
            int numInputs) {

    static void processTask(
            ModulePortScheduler &scheduler,
            std::vector<Module> &modules,
            std::vector<int> const &modulePortToModule) {

        while (true) {
            LockHandle schedulerLock = scheduler.waitPending();

            int modulePort = scheduler.nextModulePort();
            modules[modulePortToModule[modulePort]].acquire(
                modulePort, std::move(schedulerLock));
        }
    }

public:
    Worker(std::vector<Module> &&modules,
           GlobalProperties const &properties):
        scheduler_(modules.size(), properties.maxQueueSize),
        modules_(modules), workerName_(properties.workerName) {

        int numModulePorts = 0;
        for (int moduleNum = 0; moduleNum < modules_.size(); moduleNum++) {
            int numInputs = modules_[moduleNum].numInputs();
            for (int i = 0; i < numInputs; i++) {
                modulePortToModule_.push_back(moduleNum);
            }
        }
    }

    void run() {
        std::thread worldIn{[&]() {
            Worker::worldInTask(
                scheduler_, workerName_,
                modules_[WORLD_IN_MODULE_PORT].numInputs());
        }};
        worldIn.detach();

        std::thread process{[&]() {
            Worker::processTask(scheduler_, modules_, modulePortToModule_);
        }};
        process.detach();
    }
};

#endif