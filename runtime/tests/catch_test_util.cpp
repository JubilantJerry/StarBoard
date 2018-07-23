#include "catch_test_util.hpp"

void BooleanFlag::setAndSignal(bool value) {
    std::lock_guard<std::mutex> lock{flagMutex_};
    flag_.store(value);
    flagCV_.notify_all();
}

void BooleanFlag::waitForValue(bool value, int timeOutMs) {
    time_point startTime = system_clock::now();
    while (flag_.load() != value) {
        flagCV_.wait_for(flagLock_, milliseconds(timeOutMs));
        if (system_clock::now() - startTime > milliseconds(timeOutMs)) {
            FAIL("Timed out waiting for flag");
        }
    }
}

std::string const buildDir = []() {
    int constexpr charBufferSize = 512;
    char charBuffer[charBufferSize];

    int charsRead = readlink("/proc/self/exe", charBuffer, charBufferSize - 1);
    if (charsRead == -1) {
        throw std::runtime_error("Cannot find build directory");
    }
    charBuffer[charsRead] = '\0';

    std::string path{charBuffer};
    int slashPos;
    slashPos = path.find_last_of("/");
    path.erase(slashPos, std::string::npos);
    slashPos = path.find_last_of("/");
    path.erase(slashPos + 1, std::string::npos);

    return path;
}();

void childExit(int status) {
    std::string childExitFile = buildDir + std::string("bin/child_exit");

    char const *args[3] = {
        childExitFile.c_str(),
        std::to_string(status).c_str(),
        nullptr
    };

    if (execvp(childExitFile.c_str(), (char **)args) == -1) {
        exit(1);
    }
}