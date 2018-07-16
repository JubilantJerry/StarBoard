#ifndef DATA_SERIALIZER_HPP
#define DATA_SERIALIZER_HPP

#include <memory>

#include "data_interface.hpp"
#include "stream_handle.hpp"

#define MAGIC_NUMBER 0x005A6A53

enum Keyword {
    MESSAGE,
    DATA_BLOCK,
    INT_TENSOR,
    FLOAT_TENSOR,
    BRANCH
};

class DataSerializer: public DataVisitor {
private:
    StreamHandle &stream_;

public:
    DataSerializer(StreamHandle &stream): stream_(stream) {}

    virtual void visitIntTensor(IntTensor const &data) final override;
    virtual void visitFloatTensor(FloatTensor const &data) final override;
    virtual void visitBranch(Branch const &data) final override;
};

extern DataPtr deserializeData(StreamHandle &stream);

extern void serializeMessage(StreamHandle &stream, DataPtr const &message);

extern DataPtr deserializeMessage(StreamHandle &stream);

extern void serializeDataBlock(StreamHandle &stream, DataBlock const &block);

extern DataBlock deserializeDataBlock(StreamHandle &stream);

#endif