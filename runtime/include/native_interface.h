/** @file test_lib.hpp
 *  @brief A test header file
 */

#ifndef NATIVE_INTERFACE_H
#define NATIVE_INTERFACE_H

#define I(multiplier, index, remain) (multiplier * index + remain)

typedef struct NumSizes {
    int value;
} NumSizes;

typedef struct IntTensorStruct {
    int numSizes;
    int *sizes;
    int *contents;
} IntTensorStruct;

typedef struct FloatTensorStruct {
    int numSizes;
    int *sizes;
    float *contents;
} FloatTensorStruct;

typedef struct DataBlock DataBlock;

void getInputIntTensor(
        DataBlock *block, int inPortNum, IntTensorStruct *tensor);

#endif