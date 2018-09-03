#ifndef DATA_REFERENCE_HPP
#define DATA_REFERENCE_HPP

#include <atomic>

#include "data.hpp"

class DataReference {
private:
    DataSharedPtr buffers[2];
    mutable std::atomic_flag locks[2] = {ATOMIC_FLAG_INIT, ATOMIC_FLAG_INIT};
    std::atomic<bool> readSelector{0};

    DataSharedPtr const acquire(int index) const;

public:
    DataSharedPtr setWriteAcquire(DataPtr &&data) noexcept;

    bool canDirectWriteAcquire() const noexcept;

    DataSharedPtr directWriteAcquire() noexcept {
        return acquire(!readSelector.load());
    }

    void writeFinalize() noexcept {
        readSelector.store(!readSelector.load());
    }

    DataSharedPtr const readAcquire() const noexcept {
        return acquire(readSelector.load());
    }
};

#endif