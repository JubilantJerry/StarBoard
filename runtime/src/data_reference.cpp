#include "data_reference.hpp"

DataSharedPtr const DataReference::acquire(int index) const {
    std::atomic_flag &lock = locks[index];

    while (lock.test_and_set());
    DataSharedPtr buffer = buffers[index];
    lock.clear();

    return buffer;
}

DataSharedPtr DataReference::setWriteAcquire(DataPtr &&data) noexcept {
    int index = !readSelector.load();
    std::atomic_flag &lock = locks[index];
    DataSharedPtr buffer = std::move(data);

    while (lock.test_and_set());
    buffers[index] = buffer;
    lock.clear();

    return buffer;
}

bool DataReference::canDirectWriteAcquire() const noexcept {
    int index = !readSelector.load();
    std::atomic_flag &lock = locks[index];

    while (lock.test_and_set());
    bool result = (buffers[index].get() != nullptr);
    lock.clear();

    return result;
}