#ifndef STREAM_UTILS_HPP
#define STREAM_UTILS_HPP

#include "stream_handle.hpp"

enum StreamKeyword {
    DATA_BLOCK,
    INT_TENSOR,
    FLOAT_TENSOR,
    BRANCH,
    MODULE_PORT_NUM,
    MESSAGE
};

inline void serializeInt(StreamHandle &stream, int value) {
    stream.write((char const *)&value, sizeof(int));
}

inline int deserializeInt(StreamHandle &stream) {
    int value;
    stream.read((char *)&value, sizeof(int));
    return value;
}

class StreamInvalidDataException: public StreamException {
public:
    StreamInvalidDataException(std::string explanation)
        : StreamException("Stream received invalid data", explanation) {}
};

#endif