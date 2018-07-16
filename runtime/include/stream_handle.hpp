#ifndef SOCKET_HANDLES_HPP
#define SOCKET_HANDLES_HPP

#include <string>
#include <memory>
#include <sstream>
#include <unistd.h>

class StreamHandle {
public:
    virtual ~StreamHandle() = default;

    virtual void write(char const *data, int count) = 0;

    virtual void read(char *data, int count) = 0;
};

using StreamHandlePtr = std::unique_ptr<StreamHandle>;

class StringStreamHandle: public StreamHandle {
private:
    std::stringstream stream_;

public:
    StringStreamHandle(): stream_() {}

    virtual void write(char const *data, int count) {
        stream_.write(data, count);
    }

    virtual void read(char *data, int count) {
        stream_.read(data, count);
    }
};

class LocalStreamHandle: public StreamHandle {
private:
    int connectionFd_;

    LocalStreamHandle(int connectionFd): connectionFd_(connectionFd) {}

public:
    virtual ~LocalStreamHandle();

    LocalStreamHandle(LocalStreamHandle const&) = delete;
    LocalStreamHandle(LocalStreamHandle&&) noexcept = default;
    LocalStreamHandle& operator=(LocalStreamHandle const&) = delete;
    LocalStreamHandle& operator=(LocalStreamHandle&&) noexcept = default;

    static StreamHandlePtr serverEnd(std::string endpointName);

    static StreamHandlePtr clientEnd(std::string endpointName);

    virtual void write(char const *data, int count);

    virtual void read(char *data, int count);
};

#endif