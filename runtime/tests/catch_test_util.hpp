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

#define BIN_DIR "build/bin/"

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

inline void childExit(int status) {
    std::string childExitFile = BIN_DIR + std::string("child_exit");

    char const *args[3] = {
        childExitFile.c_str(),
        std::to_string(status).c_str(),
        nullptr
    };

    if (execvp(childExitFile.c_str(), (char **)args) == -1) {
        exit(1);
    }
}

template<typename ChildFunction, typename ParentFunction>
inline void forkChild(
        ChildFunction childFunction, ParentFunction parentFunction,
        int timeOutMs = 1000) {

    int processId = fork();

    if (processId == -1) {
        FAIL("Fork unsuccessful");
    } else if (processId == 0) {
        try {
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
        bool parentSuccess = parentFunction();

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