#ifndef DATA_SERIALIZER_HPP
#define DATA_SERIALIZER_HPP

#include <memory>

#include "data_interface.hpp"

#define MAGIC_NUM 0x005A6A53

enum Keyword {
    MESSAGE,
    DATA_BLOCK,
    INT_TENSOR,
    FLOAT_TENSOR,
    BRANCH
};

template<typename StreamType>
class DataSerializer: public DataVisitor {
private:
    StreamType &stream_;

public:
    DataSerializer(StreamType &stream): stream_(stream) {}

    virtual void visitIntTensor(IntTensor const &data) final override;
    virtual void visitFloatTensor(FloatTensor const &data) final override;
    virtual void visitBranch(Branch const &data) final override;
};

template<typename StreamType>
extern DataPtr deserializeData(StreamType &stream);

template<typename StreamType>
extern void serializeMessage(StreamType &stream, DataPtr const &message);

template<typename StreamType>
extern DataPtr deserializeMessage(StreamType &stream);

template<typename StreamType>
extern void serializeDataBlock(StreamType &stream, DataBlock const &block);

template<typename StreamType>
extern DataBlock deserializeDataBlock(StreamType &stream);

#include "data_serializer_impl.hpp"

#endif