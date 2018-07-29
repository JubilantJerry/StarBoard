#ifndef CATCH_TEST_UTIL_HPP
#define CATCH_TEST_UTIL_HPP

#include <iostream>
#include <string>
#include <exception>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "catch.hpp"

extern std::string const buildDir;

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

    void setAndSignal(bool value);

    void waitForValue(bool value, int timeOutMs = 1000);

    bool value() {
        return flag_.load();
    }
};

using TicValue = std::chrono::system_clock::time_point;

inline TicValue tic() {
    return std::chrono::system_clock::now();
}

inline long tocMs(TicValue ticValue) {
    using namespace std::chrono;

    return duration_cast<milliseconds>(system_clock::now() - ticValue).count();
}

inline long tocUs(TicValue ticValue) {
    using namespace std::chrono;

    return duration_cast<microseconds>(system_clock::now() - ticValue).count();
}

inline void sleepMs(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

extern void childExit(int status);

static void killHandler(int) {
    childExit(0);
}

void suspendChild(int childPid);

void resumeChild(int childPid);

template<typename ChildFunction, typename ParentFunction>
void forkChild(
        ChildFunction childFunction, ParentFunction parentFunction,
        bool killChild = false, int timeOutMs = 1000) {

    int processId = fork();

    if (processId == -1) {
        FAIL("Fork unsuccessful");
    } else if (processId == 0) {
        try {
            if (killChild) {
                signal(SIGUSR1, &killHandler);
            }

            std::thread thread{[&] {
                sleepMs(timeOutMs);
                childExit(1);
            }};
            thread.detach();

            bool childSuccess = childFunction();
            childExit(childSuccess ? 0 : 1);

        } catch(std::exception const &e) {
            std::cout << "Child exception: " << e.what() << std::endl;
            childExit(1);
        } catch (...) {
            std::cout << "Child exception: [unknown]" << std::endl;
            childExit(1);
        }
    } else {
        bool parentSuccess = parentFunction(processId);

        if (killChild) {
            kill(processId, SIGUSR1);
        }

        int waitStatus;

        if (waitpid(processId, &waitStatus, 0) == -1) {
            FAIL("Failed to wait for child process");
        }

        if (!WIFEXITED(waitStatus) || WEXITSTATUS(waitStatus) != 0) {
            FAIL("Failure detected in child process");
        }

        REQUIRE(parentSuccess);
    }
}

#endif