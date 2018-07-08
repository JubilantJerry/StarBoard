#ifndef THREADED_TEST_UTIL
#define THREADED_TEST_UTIL

#include <chrono>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include "catch.hpp"

class BooleanFlag {
private:
    std::mutex flagMutex_;
    std::unique_lock<std::mutex> flagLock_;
    std::condition_variable flagCV_;
    std::atomic<bool> flag_;

    using milliseconds = std::chrono::milliseconds;
    using system_clock = std::chrono::system_clock;
    using time_point = std::chrono::system_clock::time_point;

public:
    BooleanFlag(bool value): flagLock_(flagMutex_), flag_(value) {}

    void setAndSignal(bool value) {
        std::lock_guard<std::mutex> lock{flagMutex_};
        flag_.store(value);
        flagCV_.notify_all();
    }

    void waitForValue(bool value, int timeOutMs = 1000) {
        time_point startTime = system_clock::now();
        while (flag_.load() != value) {
            flagCV_.wait_for(flagLock_, milliseconds(timeOutMs));
            if (system_clock::now() - startTime > milliseconds(timeOutMs)) {
                FAIL("Timed out waiting for flag");
            }
        }
    }

    bool value() {
        return flag_.load();
    }
};

using TicValue = std::chrono::system_clock::time_point;

TicValue tic() {
    return std::chrono::system_clock::now();
}

long tocMs(TicValue ticValue) {
    using namespace std::chrono;

    return duration_cast<milliseconds>(system_clock::now() - ticValue).count();
}

long tocUs(TicValue ticValue) {
    using namespace std::chrono;

    return duration_cast<microseconds>(system_clock::now() - ticValue).count();
}

#endif