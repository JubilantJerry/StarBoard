#ifndef STREAM_PROTOCOL_HPP
#define STREAM_PROTOCOL_HPP

#include "data_interface.hpp"
#include "stream_handle.hpp"
#include "stream_utils.hpp"

#define MAGIC_NUMBER 0x005A6A53

extern void serializeModulePortNum(StreamHandle &stream, int modulePort);

extern int deserializeModulePortNum(StreamHandle &stream);

extern void serializeMessage(StreamHandle &stream, DataPtr const &message);

extern DataPtr deserializeMessage(StreamHandle &stream);

extern void serializeDataBlock(StreamHandle &stream, DataBlock const &block);

extern DataBlock deserializeDataBlock(StreamHandle &stream);

#endif