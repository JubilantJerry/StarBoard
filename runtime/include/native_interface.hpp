#include <cstdarg>
extern "C" {
#include "native_interface.h"
}

struct numSizesArg {
    int value;
    explicit numSizesArg(int value): value(value) {};
};

class IntTensorObj: public IntTensorStruct {
private:
    using IntTensorStruct::numSizes;
    using IntTensorStruct::sizes;
    using IntTensorStruct::contents;

public:
    IntTensorObj(numSizesArg numSizesV, ...) {
        va_list sizesList;
        va_start(sizesList, numSizesV);

        numSizes = numSizesV.value;
        int totalSize = 1;
        sizes = new int[numSizesV.value];
        for (int i = 0; i < numSizesV.value; i++) {
            int size = va_arg(sizesList, int);
            sizes[i] = size;
            totalSize *= size;
        }
        contents = new int[totalSize];
    }

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