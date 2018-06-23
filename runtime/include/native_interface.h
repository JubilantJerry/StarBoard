/** @file test_lib.hpp
 *  @brief A test header file
 */

#ifndef NATIVE_INTERFACE_H
#define NATIVE_INTERFACE_H

#define INDEX(sizes, indices...) \
    __INDEX_DISPATCH(indices, \
        __INDEX_8, __INDEX_7, __INDEX_6, __INDEX_5, \
        __INDEX_4, __INDEX_3, __INDEX_2, __INDEX_1)(sizes, indices)

typedef struct NumSizes {
    int value;
} NumSizes;

typedef struct IntTensorStruct {
    int numSizes;
    int const *sizes;
    int *contents;
} IntTensorStruct;

typedef struct FloatTensorStruct {
    int numSizes;
    int const *sizes;
    float *contents;
} FloatTensorStruct;

typedef struct Branch Branch;

typedef struct DataBlock DataBlock;

int getNumInputs(DataBlock const *block);

IntTensorStruct const * getInputIntTensor(
        DataBlock const *block, int inPortNum);

FloatTensorStruct const * getInputFloatTensor(
        DataBlock const *block, int inPortNum);

Branch const * getInputBranch(
        DataBlock const *block, int inPortNum);

int getNumOutputs(DataBlock const *block);

IntTensorStruct const * makeOutputIntTensor(
        DataBlock *block, int outPortNum, NumSizes numSizesV, ...);

FloatTensorStruct const * makeOutputFloatTensor(
        DataBlock *block, int outPortNum, NumSizes numSizesV, ...);

Branch * makeOutputBranch(
        DataBlock *block, int outPortNum, int size);

IntTensorStruct const * moveToOutputIntTensor(
        DataBlock *block, int outPortNum, int inPortNum);

FloatTensorStruct const * moveToOutputFloatTensor(
        DataBlock *block, int outPortNum, int inPortNum);

Branch * moveToOutputBranch(
        DataBlock *block, int outPortNum, int inPortNum);

int getBranchSize(Branch const *branch);

IntTensorStruct const * getBranchIntTensor(
        Branch const *branch, int index);

FloatTensorStruct const * getBranchFloatTensor(
        Branch const *branch, int index);

Branch * getBranchBranch(
        Branch const *branch, int index);

#define APPEND_INDEX -1

IntTensorStruct const * makeBranchIntTensor(
        Branch *branch, int index, NumSizes numSizesV, ...);

FloatTensorStruct const * makeBranchFloatTensor(
        Branch *branch, int index, NumSizes numSizesV, ...);

Branch * makeBranchBranch(
        Branch *branch, int index, int size);

void popBranch(Branch *branch);

#define __INDEX_DISPATCH(_1, _2, _3, _4, _5, _6, _7, _8, target, ...) target

#define __INDEX_1(sizes, _1) (_1)
#define __INDEX_2(sizes, _1, _2) \
    (sizes[1] * __INDEX_1(sizes, _1) + _2)
#define __INDEX_3(sizes, _1, _2, _3) \
    (sizes[2] * __INDEX_2(sizes, _1, _2) + _3)
#define __INDEX_4(sizes, _1, _2, _3, _4) \
    (sizes[3] * __INDEX_3(sizes, _1, _2, _3) + _4)
#define __INDEX_5(sizes, _1, _2, _3, _4, _5) \
    (sizes[4] * __INDEX_4(sizes, _1, _2, _3, _4) + _5)
#define __INDEX_6(sizes, _1, _2, _3, _4, _5, _6) \
    (sizes[5] * __INDEX_5(sizes, _1, _2, _3, _4, _5) + _6)
#define __INDEX_7(sizes, _1, _2, _3, _4, _5, _6, _7) \
    (sizes[6] * __INDEX_6(sizes, _1, _2, _3, _4, _5, _6) + _7)
#define __INDEX_8(sizes, _1, _2, _3, _4, _5, _6, _7, _8) \
    (sizes[7] * __INDEX_7(sizes, _1, _2, _3, _4, _5, _6, _7) + _8)

#endif