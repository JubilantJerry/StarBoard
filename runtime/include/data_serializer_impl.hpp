#include <endian.h>
#include <custom_utility.hpp>

#include "data_serializer.hpp"


template<typename StreamType>
static inline void serializeInt(StreamType &stream, int value) {
    stream.write((char const *)&value, sizeof(int));
}

template<typename StreamType, typename TensorType>
static int serializeSizes(StreamType &stream, TensorType const &data) {
    int numSizes = data.numSizes();
    int const *sizes = data.sizes();
    int totalSize = 1;

    serializeInt(stream, numSizes);

    for (int i = 0; i < numSizes; i++) {
        int size = sizes[i];
        serializeInt(stream, size);
        totalSize *= size;
    }

    return totalSize;
}

template<typename StreamType>
static void serializePayload(
        StreamType &stream, char const *payload, int size) {

    stream.write(payload, size);
}

template<typename StreamType>
static inline int deserializeInt(StreamType &stream) {
    int value;
    stream.read((char *)&value, sizeof(int));
    return value;
}

template<typename StreamType>
static inline int deserializeSizes
        (StreamType &stream, int &numSizes, std::unique_ptr<int[]> &sizes) {

    numSizes = deserializeInt(stream);
    sizes = make_unique<int[]>((size_t)numSizes);
    int totalSize = 1;

    for (int i = 0; i < numSizes; i++) {
        int size = deserializeInt(stream);
        sizes[i] = size;
        totalSize *= size;
    }

    return totalSize;
}

template<typename StreamType>
static void deserializePayload(StreamType &stream, char *payload, int size) {
    stream.read(payload, size);
}

template<typename StreamType>
void DataSerializer<StreamType>::visitIntTensor(IntTensor const &data) {
    serializeInt(stream_, Keyword::INT_TENSOR);

    int totalSize = serializeSizes(stream_, data);

    serializePayload(stream_, (char const *)data.contents(),
        totalSize * sizeof(int));
}

template<typename StreamType>
void DataSerializer<StreamType>::visitFloatTensor(FloatTensor const &data) {
    serializeInt(stream_, Keyword::FLOAT_TENSOR);

    int totalSize = serializeSizes(stream_, data);

    serializePayload(stream_, (char const *)data.contents(),
        totalSize * sizeof(float));
}

template<typename StreamType>
void DataSerializer<StreamType>::visitBranch(Branch const &data) {
    serializeInt(stream_, Keyword::BRANCH);

    int size = data.size();

    serializeInt(stream_, size);

    for (int i = 0; i < size; i++) {
        *this & *data.getValue(i);
    }
}

template<typename StreamType>
DataPtr deserializeData(StreamType &stream) {
    Keyword dataType = static_cast<Keyword>(deserializeInt(stream));

    switch (dataType) {
        case Keyword::INT_TENSOR: {
            int numSizes;
            std::unique_ptr<int[]> sizes;

            int totalSize = deserializeSizes(stream, numSizes, sizes);

            std::unique_ptr<int[]> contents =
                make_unique<int[]>(totalSize);

            deserializePayload(stream, (char *)contents.get(),
                totalSize * sizeof(int));

            return make_unique<IntTensor>(
                IntTensorM{numSizes, sizes.release(), contents.release()});
        }

        case Keyword::FLOAT_TENSOR: {
            int numSizes;
            std::unique_ptr<int[]> sizes;

            int totalSize = deserializeSizes(stream, numSizes, sizes);

            std::unique_ptr<float[]> contents =
                make_unique<float[]>(totalSize);

            deserializePayload(stream, (char *)contents.get(),
                totalSize * sizeof(int));

            return make_unique<FloatTensor>(
                FloatTensorM{numSizes, sizes.release(), contents.release()});
        }

        case Keyword::BRANCH: {
            int size = deserializeInt(stream);

            std::unique_ptr<Branch> data = make_unique<Branch>(size);

            for (int i = 0; i < size; i++) {
                data->setValue(i, deserializeData(stream));
            }

            return std::move(data);
        }

        default: throw std::runtime_error("Invalid serialization");
    }
}

// template<typename StreamType>
// void serializeDataBlock(StreamType &stream, DataBlock &block);

// template<typename StreamType>
// DataBlock deserializeDataBlock(StreamType &stream);