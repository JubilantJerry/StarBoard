#ifndef SOCKET_HANDLES_HPP
#define SOCKET_HANDLES_HPP

#include <string>
#include <memory>
#include <sstream>
#include <atomic>

class StreamException: public std::exception {
private:
    std::string exceptionText_;

public:
    StreamException(std::string errorHeader, std::string explanation)
        : exceptionText_(errorHeader + ": " + explanation) {}

    virtual char const *what() const noexcept final {
        return exceptionText_.c_str();
    }
};

class StreamInitializationException: public StreamException {
public:
    StreamInitializationException(std::string explanation)
        : StreamException("Stream initialization failure", explanation) {}
};

class StreamInterruptedException: public StreamException {
public:
    StreamInterruptedException(std::string explanation)
        : StreamException("Stream interrupted", explanation) {}
};

class StreamHandle {
private:
    std::atomic<bool> isDisabled_{false};

protected:
    void checkDisabled() {
        if (isDisabled_.load()) {
            throw StreamInterruptedException("stream has closed");
        }
    }

public:
    virtual ~StreamHandle() = default;

    virtual void write(char const *data, int count) = 0;

    virtual void read(char *data, int count) = 0;

    virtual void disable() noexcept {
        isDisabled_.store(true);
    }
};

using StreamHandlePtr = std::unique_ptr<StreamHandle>;

class StringStreamHandle: public StreamHandle {
private:
    std::stringstream stream_;

public:
    StringStreamHandle(): stream_() {}

    virtual void write(char const *data, int count) override {
        checkDisabled();
        stream_.write(data, count);
    }

    virtual void read(char *data, int count) override {
        checkDisabled();
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
    LocalStreamHandle(LocalStreamHandle&&) = default;
    LocalStreamHandle& operator=(LocalStreamHandle const&) = delete;
    LocalStreamHandle& operator=(LocalStreamHandle&&) = default;

    static StreamHandlePtr serverEnd(std::string endpointName);

    static StreamHandlePtr clientEnd(std::string endpointName);

    virtual void write(char const *data, int count) override;

    virtual void read(char *data, int count) override;

    virtual void disable() noexcept override;
};



#endif