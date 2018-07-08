#ifndef MODULE_HPP
#define MODULE_HPP

#include "boost/circular_buffer.hpp"

#include "data.hpp"

class MessageQueue {
private:
    boost::circular_buffer<DataPtr> queue;
public:
    MessageQueue(int maxSize): queue(maxSize) {}

    void enqueue(DataPtr &&data) {
        queue.push_back(std::move(data));
    }

    DataPtr dequeue() {
        DataPtr result = std::move(queue.front());
        queue.pop_front();
        return result;
    }

    int size() {
        return queue.size();
    }
};

#endif