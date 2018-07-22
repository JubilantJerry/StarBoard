#ifndef DATA_SERIALIZER_HPP
#define DATA_SERIALIZER_HPP

#include <memory>

#include "data_interface.hpp"
#include "stream_handle.hpp"

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

#endif