#ifndef MODULE_PORT_SCHEDULER_H
#define MODULE_PORT_SCHEDULER_H

#include <vector>
#include <list>
#include <mutex>
#include <boost/circular_buffer.hpp>
#include <boost/pool/pool_alloc.hpp>

#include "data.hpp"

class MessageQueue {
private:
    boost::circular_buffer<DataPtr> queue_;
public:
    MessageQueue(): MessageQueue(0) {};

    MessageQueue(int maxQueueSize): queue_(maxQueueSize) {}

    void enqueue(DataPtr &&data) {
        queue_.push_back(std::move(data));
    }

    DataPtr dequeue() {
        DataPtr result = std::move(queue_.front());
        queue_.pop_front();
        return result;
    }

    int size() {
        return queue_.size();
    }
};

using LockHandle = std::unique_lock<std::mutex>;

class ModulePortScheduler {
private:
    int numDataReady_;
    int numModulePortsReady_;
    int numModulePortsPending_;

    std::vector<int> dataReady_;
    std::vector<int> modulePortReady_;
    std::vector<int> modulePortPending_;

    std::list<int, boost::fast_pool_allocator<int>> pendingList_;
    std::vector<std::list<int>::iterator> pendingListIterators_;

    std::vector<MessageQueue> queues_;
    std::mutex mutex_;

    void updateModulePortsPending(int modulePort);

public:
    ModulePortScheduler(int numModules, int maxQueueSize);

    int numDataReady() noexcept {
        return numDataReady_;
    }

    int numModulePortsReady() noexcept {
        return numModulePortsReady_;
    }

    int numModulePortsPending() noexcept {
        return numModulePortsPending_;
    }

    bool dataReady(int modulePort) {
        return dataReady_[modulePort];
    }

    bool modulePortReady(int modulePort) {
        return modulePortReady_[modulePort];
    }

    bool modulePortPending(int modulePort) {
        return modulePortPending_[modulePort];
    }

    void setDataReady(int modulePort, bool value);

    void setModulePortReady(int modulePort, bool value);

    int nextModulePort();

    MessageQueue &getQueue(int modulePort) {
        return queues_[modulePort];
    }

    LockHandle lock() {
        return LockHandle{mutex_};
    }
};

#endif