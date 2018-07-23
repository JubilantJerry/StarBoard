#include <iostream>
#include <stdexcept>
#include <chrono>
#include <thread>
#include <custom_utility.hpp>
#include <unistd.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <signal.h>

#include "stream_handle.hpp"

int constexpr INIT_WAIT_US = 10;
int constexpr CONNECT_SLEEP_MULTIPLIER = 2;
int constexpr CONNECT_MAX_SLEEP_US = 1000000;
int constexpr SEND_MAX_WAIT_S = 1;

static void *disableSIGPIPE = []() {
    sigignore(SIGPIPE);
    return nullptr;
}();

StreamHandlePtr LocalStreamHandle::serverEnd(std::string endpointName) {
    int listenFd = -1;
    int connectionFd = -1;
    struct sockaddr_un address{};
    struct timeval sendTimeout{};

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

    sendTimeout.tv_sec = SEND_MAX_WAIT_S;
    setsockopt(
        connectionFd, SOL_SOCKET, SO_SNDTIMEO,
        &sendTimeout, sizeof(sendTimeout));

    close(listenFd);
    return StreamHandlePtr{new LocalStreamHandle{connectionFd}};

    error:
    if (listenFd != -1) {
        close(listenFd);
    }
    if (connectionFd != -1) {
        close(connectionFd);
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
    int connectSleepUs = 0;

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
                sleepUs(connectSleepUs);
                
                if (connectSleepUs == 0) {
                    connectSleepUs = INIT_WAIT_US;
                } else {
                    connectSleepUs *= CONNECT_SLEEP_MULTIPLIER;
                    if (connectSleepUs > CONNECT_MAX_SLEEP_US) {
                        connectSleepUs = CONNECT_MAX_SLEEP_US;
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

        checkDisabled();
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

        checkDisabled();
        int bytesRead = read(connectionFd_, data, count);

        if (bytesRead == -1) {
            throw StreamInterruptedException(std::string(strerror(errno)));
        }

        data += bytesRead;
        count -= bytesRead;
    }
}

void LocalStreamHandle::disable() noexcept {
    StreamHandle::disable();
    int result = close(connectionFd_);
    connectionFd_ = -1;
    if (result == -1) {
        throw StreamInterruptedException(std::string(strerror(errno)));
    }
}

LocalStreamHandle::~LocalStreamHandle() {
    if (connectionFd_ != -1) {
        int result = close(connectionFd_);
        if (result == -1) {
            throw StreamInterruptedException(std::string(strerror(errno)));
        }
    }
}