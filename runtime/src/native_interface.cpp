#include "native_interface.hpp"

inline int fillSizes(int numSizes, va_list sizesList, int *sizes) {
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

void getInputIntTensor(
        DataBlock *block, int inPortNum, IntTensorStruct *tensor) {}