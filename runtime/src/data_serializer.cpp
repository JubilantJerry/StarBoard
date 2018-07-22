#include <stdexcept>
#include <endian.h>
#include <custom_utility.hpp>

#include "stream_utils.hpp"
#include "data_serializer.hpp"

template<typename StreamHandle, typename TensorType>
static int serializeSizes(StreamHandle &stream, TensorType const &data) {
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

static void serializePayload(
        StreamHandle &stream, char const *payload, int size) {

    stream.write(payload, size);
}

static inline int deserializeSizes
        (StreamHandle &stream, int &numSizes, std::unique_ptr<int[]> &sizes) {

    numSizes = deserializeInt(stream);
    sizes = make_unique<int[]>(numSizes);
    int totalSize = 1;

    for (int i = 0; i < numSizes; i++) {
        int size = deserializeInt(stream);
        sizes[i] = size;
        totalSize *= size;
    }

    return totalSize;
}

static void deserializePayload(StreamHandle &stream, char *payload, int size) {
    stream.read(payload, size);
}

void DataSerializer::visitIntTensor(IntTensor const &data) {
    serializeInt(stream_, StreamKeyword::INT_TENSOR);

    int totalSize = serializeSizes(stream_, data);

    serializePayload(stream_, (char const *)data.contents(),
        totalSize * sizeof(int));
}

void DataSerializer::visitFloatTensor(FloatTensor const &data) {
    serializeInt(stream_, StreamKeyword::FLOAT_TENSOR);

    int totalSize = serializeSizes(stream_, data);

    serializePayload(stream_, (char const *)data.contents(),
        totalSize * sizeof(float));
}

void DataSerializer::visitBranch(Branch const &data) {
    serializeInt(stream_, StreamKeyword::BRANCH);

    int size = data.size();

    serializeInt(stream_, size);

    for (int i = 0; i < size; i++) {
        *this & *data.getValue(i);
    }
}

DataPtr deserializeData(StreamHandle &stream) {
    StreamKeyword dataType =
        static_cast<StreamKeyword>(deserializeInt(stream));

    switch (dataType) {
        case StreamKeyword::INT_TENSOR: {
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

        case StreamKeyword::FLOAT_TENSOR: {
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

        case StreamKeyword::BRANCH: {
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
