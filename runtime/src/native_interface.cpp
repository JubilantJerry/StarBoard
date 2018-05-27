#include "custom_utility.hpp"

#include "native_interface.hpp"

static inline int fillSizes(int numSizes, va_list sizesList, int *sizes) {
    int totalSize = 1;
    for (int i = 0; i < numSizes; i++) {
        int size = va_arg(sizesList, int);
        sizes[i] = size;
        totalSize *= size;
    }
    return totalSize;
}

inline void IntTensorObj::fillSelf(
        NumSizes numSizesV, va_list sizesList) {

    numSizes = numSizesV.value;
    sizes = new int[numSizes];
    int totalSize = fillSizes(numSizes, sizesList, sizes);
    contents = new int[totalSize];
}

IntTensorObj::IntTensorObj(NumSizes numSizesV, ...) {
    va_list sizesList;
    va_start(sizesList, numSizesV);
    fillSelf(numSizesV, sizesList);
    va_end(sizesList);
}

IntTensorObj::IntTensorObj(NumSizes numSizesV, va_list sizesList) {
    fillSelf(numSizesV, sizesList);
}

std::ostream& IntTensorObj::print(std::ostream &stream) const {
    stream << "IntTensorObj";
    stream << std::make_pair(sizes, sizes + numSizes);
    return stream;
}

inline void FloatTensorObj::fillSelf(
        NumSizes numSizesV, va_list sizesList) {

    numSizes = numSizesV.value;
    sizes = new int[numSizes];
    int totalSize = fillSizes(numSizes, sizesList, sizes);
    contents = new float[totalSize];
}

FloatTensorObj::FloatTensorObj(NumSizes numSizesV, ...) {
    va_list sizesList;
    va_start(sizesList, numSizesV);
    fillSelf(numSizesV, sizesList);
    va_end(sizesList);
}

FloatTensorObj::FloatTensorObj(NumSizes numSizesV, va_list sizesList) {
    fillSelf(numSizesV, sizesList);
}

std::ostream& FloatTensorObj::print(std::ostream &stream) const {
    stream << "FloatTensorObj";
    stream << std::make_pair(sizes, sizes + numSizes);
    return stream;
}

DataBlock::DataBlock(int numInputs, int numStateData, int numOutputs)
    :inputs(std::vector<DataPtr>(numInputs)) {}

IntTensorStruct const * getInputIntTensor(DataBlock *block, int inPortNum) {
    return dynamic_cast<IntTensorObj const *>(
        block->inputs[inPortNum].get());
}