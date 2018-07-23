#ifndef MODULE_PORT_SCHEDULER_H
#define MODULE_PORT_SCHEDULER_H

#include <vector>
#include <list>
#include <atomic>
#include <mutex>
#include <condition_variable>
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


class SchedulingQueue {
private:
    std::list<int, boost::fast_pool_allocator<int>> queue_;

public:
    using QueuePlace = std::list<int>::const_iterator;

    int size() const {
        return queue_.size();
    }

    QueuePlace enqueue(int value) {
        queue_.push_back(value);
        return --queue_.end();
    }

    int dequeue() {
        int value = queue_.front();
        queue_.pop_front();
        return value;
    }

    void remove(QueuePlace place) {
        queue_.erase(place);
    }

    QueuePlace end() const {
        return queue_.end();
    }
};


class ModulePortScheduler {
private:
    int numDataReady_;
    int numModulePortsReady_;
    int numModulePortsPending_;

    std::vector<int> dataReady_;
    std::vector<int> modulePortReady_;
    std::vector<int> modulePortPending_;

    std::vector<int> pendingQueueAssignment_;
    std::vector<SchedulingQueue> pendingQueues_;
    std::vector<SchedulingQueue::QueuePlace> pendingQueuePlaces_;

    std::vector<MessageQueue> queues_;

    std::vector<std::mutex> lockMutexes_;
    std::vector<std::condition_variable> waitPendingCVs_;

    void updateModulePortsPending(int modulePort);

public:
    ModulePortScheduler(
        int numModulePorts, int numPendingQueues, int maxQueueSize,
        std::vector<int> &&pendingQueueAssignment);

    int numModulePorts() noexcept {
        return queues_.size();
    }

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

    int nextModulePort(int queueNum);

    int pendingQueueAssignment(int modulePort) {
        return pendingQueueAssignment_[modulePort];
    }

    MessageQueue &getQueue(int modulePort) {
        return queues_[modulePort];
    }

    LockHandle lock(int queueNumber) {
        return LockHandle{lockMutexes_[queueNumber]};
    }

    LockHandle waitPending(int queueNumber) {
        LockHandle schedulerLock = lock(queueNumber);
        SchedulingQueue &pendingQueue = pendingQueues_[queueNumber];

        while (pendingQueue.size() == 0) {
            waitPendingCVs_[queueNumber].wait(schedulerLock);
        }

        return schedulerLock;
    }
};

#endif