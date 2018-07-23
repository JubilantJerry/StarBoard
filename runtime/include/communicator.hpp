#ifndef WORLD_INTERFACE_HPP
#define WORLD_INTERFACE_HPP

#include <string>
#include <utility>
#include <unordered_map>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include "stream_handle.hpp"
#include "stream_protocol.hpp"
#include "module_port_scheduler.hpp"

char constexpr WORLD_ENDPOINT_SUFFIX[] = "_world";

class Communicator {
private:
    using LockGuard = std::lock_guard<std::mutex>;
    using UniqueLock = std::unique_lock<std::mutex>;

    StreamHandlePtr stream_;
    std::mutex streamMutex_;

    std::condition_variable streamCreatedCV_;
    UniqueLock streamLock_{streamMutex_};

    class PrepareSendHandle;

protected:
    virtual StreamHandlePtr makeStream() = 0;

    virtual void receive(StreamHandle &stream) = 0;

    virtual void prepareSend() = 0;

    virtual void undoPrepareSend() = 0;

    virtual void send(StreamHandle &stream) = 0;

public:

    void receiveTask();

    void sendTask();

    virtual ~Communicator() noexcept = default;
};

class WorldCommunicator: public Communicator {
private:
    std::string workerName_;
    ModulePortScheduler &scheduler_;
    int queueNumber_;
    LockHandle lock_;

protected:
    virtual StreamHandlePtr makeStream() {
        return LocalStreamHandle::serverEnd(
            workerName_ + WORLD_ENDPOINT_SUFFIX);
    }

    virtual void receive(StreamHandle &stream);

    virtual void prepareSend() {
        lock_ = scheduler_.waitPending(queueNumber_);
    }

    virtual void undoPrepareSend() {
        lock_.unlock();
    }

    virtual void send(StreamHandle &stream);

public:
    WorldCommunicator(
            std::string workerName,
            ModulePortScheduler &scheduler,
            int queueNumber)
        : workerName_(workerName),
          scheduler_(scheduler),
          queueNumber_(queueNumber) {}
};

#endif