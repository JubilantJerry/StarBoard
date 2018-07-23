#include "communicator.hpp"

class Communicator::PrepareSendHandle {
private:
    Communicator &communicator_;

public:
    PrepareSendHandle(Communicator &communicator)
        : communicator_(communicator) {

        communicator_.prepareSend();
    }

    ~PrepareSendHandle() {
        communicator_.undoPrepareSend();
    }
};

void Communicator::receiveTask() {
    while (true) {
        {
            LockGuard lock{streamMutex_};
            stream_ = makeStream();
            streamCreatedCV_.notify_all();
        }

        try {
            while (true) {
                receive(*stream_);
            }
        } catch (StreamInterruptedException const &) {
            // TODO: log the error somehow
            stream_->disable();
        }
    }
}

void Communicator::sendTask() {
    while (true) {
        streamCreatedCV_.wait(streamLock_);
        streamLock_.unlock();

        try {
            while (true) {
                PrepareSendHandle prepareSendHandle{*this};
                LockGuard lock{streamMutex_};
                send(*stream_);
            }
        } catch (StreamException const &) {
            // TODO: log the error somehow
            stream_->disable();
        }

        streamLock_.lock();
    }
}

void WorldCommunicator::receive(StreamHandle &stream) {
    int modulePort = receiveModulePort(stream);
    DataPtr message = receiveMessage(stream);

    LockHandle lock = scheduler_.lock(
        scheduler_.pendingQueueAssignment(modulePort));
    MessageQueue &queue = scheduler_.getQueue(modulePort);
    queue.enqueue(std::move(message));
    scheduler_.setDataReady(modulePort, true);
}

void WorldCommunicator::send(StreamHandle &stream) {
    int modulePort = scheduler_.nextModulePort(queueNumber_);
    MessageQueue &queue = scheduler_.getQueue(modulePort);
    DataPtr data = queue.dequeue();
    if (queue.size() == 0) {
        scheduler_.setDataReady(modulePort, false);
    }

    sendModulePort(stream, modulePort);
    sendMessage(stream, data);
}