#include "stream_utils.hpp"
#include "data_serializer.hpp"
#include "stream_protocol.hpp"

static inline void checkMagicNumber(StreamHandle &stream) {
    if (deserializeInt(stream) != MAGIC_NUMBER) {
        throw std::runtime_error("Serializer version mismatch");
    }
}

void sendModulePort(StreamHandle &stream, int modulePort) {
    serializeInt(stream, MAGIC_NUMBER);
    serializeInt(stream, StreamKeyword::MODULE_PORT_NUM);
    serializeInt(stream, modulePort);
}

void sendMessage(StreamHandle &stream, DataPtr const &message) {
    serializeInt(stream, MAGIC_NUMBER);
    serializeInt(stream, StreamKeyword::MESSAGE);
    DataSerializer{stream} & *message;
}

DataPtr receiveMessage(StreamHandle &stream) {
    checkMagicNumber(stream);

    if (deserializeInt(stream) != StreamKeyword::MESSAGE) {
        throw StreamInvalidDataException(
            "message deserialization error");
    }
    return deserializeData(stream);
}

int receiveModulePort(StreamHandle &stream) {
    checkMagicNumber(stream);

    if (deserializeInt(stream) != StreamKeyword::MODULE_PORT_NUM) {
        throw StreamInvalidDataException(
            "module port number deserialization error");
    }
    return deserializeInt(stream);
}