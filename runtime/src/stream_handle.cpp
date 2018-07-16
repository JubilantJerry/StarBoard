#include <custom_utility.hpp>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>

#include "stream_handle.hpp"

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
    throw std::runtime_error(
        "Failed to create socket: " + std::string(strerror(errno)));
}

StreamHandlePtr LocalStreamHandle::clientEnd(std::string endpointName) {
    int connectionFd = -1;
    struct sockaddr_un address{};

    connectionFd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (connectionFd == -1) {
        goto error;
    }

    address.sun_family = AF_UNIX;
    strncpy(address.sun_path + 1, endpointName.c_str(),
            sizeof(address.sun_path) - 2);

    if (connect(connectionFd, (struct sockaddr*)&address,
                sizeof(address)) == -1) {
        goto error;
    }

    return StreamHandlePtr{new LocalStreamHandle{connectionFd}};

    error:
    if (connectionFd != -1) {
        close(connectionFd);
    }
    throw std::runtime_error(
        "Failed to create socket: " + std::string(strerror(errno)));
}

void LocalStreamHandle::write(char const *data, int count) {
    while (count != 0) {
        using ::write;
        int bytesWritten = write(connectionFd_, data, count);
        data += bytesWritten;
        count -= bytesWritten;
    }
}

void LocalStreamHandle::read(char *data, int count) {
    while (count != 0) {
        using ::read;
        int bytesRead = read(connectionFd_, data, count);
        data += bytesRead;
        count -= bytesRead;
    }
}

LocalStreamHandle::~LocalStreamHandle() {
    int result = close(connectionFd_);
    if (result == -1) {
        throw std::runtime_error(
            "Socket shutdown failure: " + std::string(strerror(errno)));
    }
}