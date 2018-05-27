#include <cstdarg>
#include <utility>

extern "C" {
#include "native_interface.h"
}

class Data {
public:
    virtual ~Data() {}
};

class IntTensorObj final: public IntTensorStruct, public Data {
private:
    using IntTensorStruct::numSizes;
    using IntTensorStruct::sizes;
    using IntTensorStruct::contents;
    void fillSelf(NumSizes numSizesV, va_list args);

public:
    IntTensorObj() {
        numSizes = 0;
        sizes = nullptr;
        contents = nullptr;
    };
    IntTensorObj(NumSizes numSizesV, ...);
    IntTensorObj(NumSizes numSizesV, va_list args);

    ~IntTensorObj() {
        delete[] sizes;
        delete[] contents;
    }

    IntTensorObj(IntTensorObj const&) = delete;

    IntTensorObj& operator=(IntTensorObj const&) = delete;

    int getNumSizes() {
        return numSizes;
    }

    int *getSizes() {
        return sizes;
    }

    int *getContents() {
        return contents;
    }
};

class FloatTensorObj final: public FloatTensorStruct, public Data {
private:
    using FloatTensorStruct::numSizes;
    using FloatTensorStruct::sizes;
    using FloatTensorStruct::contents;
    void fillSelf(NumSizes numSizesV, va_list args);

public:
    FloatTensorObj() {
        numSizes = 0;
        sizes = nullptr;
        contents = nullptr;
    };
    FloatTensorObj(NumSizes numSizesV, ...);
    FloatTensorObj(NumSizes numSizesV, va_list sizesList);

    ~FloatTensorObj() {
        delete[] sizes;
        delete[] contents;
    }

    FloatTensorObj(IntTensorObj const&) = delete;

    FloatTensorObj& operator=(IntTensorObj const&) = delete;

    int getNumSizes() {
        return numSizes;
    }

    int *getSizes() {
        return sizes;
    }

    float *getContents() {
        return contents;
    }
};

class DataBlock final {
public:
    DataBlock(int numInputs, int numStateData, int numOutputs) {}
    void setInput(int inPortNum, Data &&tensor) {}
};