#ifndef STREAM_PROTOCOL_HPP
#define STREAM_PROTOCOL_HPP

#include "data_interface.hpp"
#include "stream_handle.hpp"
#include "stream_utils.hpp"

#define MAGIC_NUMBER 0x005A6A53

extern void sendModulePort(StreamHandle &stream, int modulePort);

extern int receiveModulePort(StreamHandle &stream);

extern void sendMessage(StreamHandle &stream, DataPtr const &message);

extern DataPtr receiveMessage(StreamHandle &stream);

extern void sendDataBlock(StreamHandle &stream, DataBlock const &block);

extern DataBlock receiveDataBlock(StreamHandle &stream);

#endif