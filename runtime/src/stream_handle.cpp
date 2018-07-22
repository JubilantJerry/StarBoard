#include <iostream>
#include <stdexcept>
#include <chrono>
#include <thread>
#include <custom_utility.hpp>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <signal.h>

#include "stream_handle.hpp"

int constexpr INIT_WAIT_US = 10;
int constexpr WAIT_MULTIPLIER = 2;
int constexpr MAX_WAIT_US = 1000000;

static void *disableSIGPIPE = []() {
    sigignore(SIGPIPE);
    return nullptr;
}();

StreamHandlePtr LocalStreamHandle::serverEnd(std::string endpointName) {
    int listenFd = -1;
    int connectionFd = -1;
    struct sockaddr_un address{};

    listenFd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (listenFd == -1) {
        goto error;
    }

    address.sun_family = AF_UNIX;
    strncpy(address.sun_path + 1, endpointName.c_str(),
            sizeof(address.sun_path) - 2);

    if (bind(listenFd, (struct sockaddr*)&address,
             sizeof(address)) == -1) {
        goto error;
    }

    if (listen(listenFd, 1) == -1) {
        goto error;
    }

    connectionFd = accept(listenFd, NULL, NULL);
    if (connectionFd == -1) {
        goto error;
    }

    close(listenFd);
    return StreamHandlePtr{new LocalStreamHandle{connectionFd}};

    error:
    if (listenFd != -1) {
        close(listenFd);
    }
    throw StreamInitializationException(std::string(strerror(errno)));
}

static void sleepUs(int us) {
    std::this_thread::sleep_for(std::chrono::microseconds(us));
}

StreamHandlePtr LocalStreamHandle::clientEnd(std::string endpointName) {
    int connectionFd = -1;
    struct sockaddr_un address{};
    bool successfulConnect = false;
    int waitUs = 0;

    while (!successfulConnect) {
        connectionFd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (connectionFd == -1) {
            goto error;
        }

        address.sun_family = AF_UNIX;
        strncpy(address.sun_path + 1, endpointName.c_str(),
                sizeof(address.sun_path) - 2);

        if (connect(connectionFd, (struct sockaddr*)&address,
                    sizeof(address)) == -1) {
            if (errno != ECONNREFUSED) {
                goto error;
            } else {
                close(connectionFd);
                sleepUs(waitUs);
                
                if (waitUs == 0) {
                    waitUs = INIT_WAIT_US;
                } else {
                    waitUs *= WAIT_MULTIPLIER;
                    if (waitUs > MAX_WAIT_US) {
                        waitUs = MAX_WAIT_US;
                    }
                }
            }
        } else {
            successfulConnect = true;
        }
    }

    return StreamHandlePtr{new LocalStreamHandle{connectionFd}};

    error:
    if (connectionFd != -1) {
        close(connectionFd);
    }
    throw StreamInitializationException(std::string(strerror(errno)));
}

void LocalStreamHandle::write(char const *data, int count) {
    while (count != 0) {
        using ::write;
        int bytesWritten = write(connectionFd_, data, count);

        if (bytesWritten == -1) {
            throw StreamInterruptedException(std::string(strerror(errno)));
        }

        data += bytesWritten;
        count -= bytesWritten;
    }
}

void LocalStreamHandle::read(char *data, int count) {
    while (count != 0) {
        using ::read;
        int bytesRead = read(connectionFd_, data, count);

        if (bytesRead == -1) {
            throw StreamInterruptedException(std::string(strerror(errno)));
        }

        data += bytesRead;
        count -= bytesRead;
    }
}

LocalStreamHandle::~LocalStreamHandle() {
    int result = close(connectionFd_);
    if (result == -1) {
        throw StreamInterruptedException(std::string(strerror(errno)));
    }
}