#ifndef NATIVE_INTERFACE_H
#define NATIVE_INTERFACE_H

#include "data.h"

typedef struct DataBlock DataBlock;

int inputMsg_getNum(DataBlock *block);

IntTensorR * inputMsg_getIntTensor(DataBlock *block);

FloatTensorR * inputMsg_getFloatTensor(DataBlock *block);

BranchR * inputMsg_getBranch(DataBlock *block);

int outputMsg_getNum(DataBlock *block);

IntTensorRW * outputMsg_makeIntTensor(
        DataBlock *block, int outPortNum, NumSizes numSizesV, ...);

FloatTensorRW * outputMsg_makeFloatTensor(
        DataBlock *block, int outPortNum, NumSizes numSizesV, ...);

BranchRW * outputMsg_makeBranch(
        DataBlock *block, int outPortNum, int size);

IntTensorRW * outputMsg_moveIntTensor(
        DataBlock *block, int outPortNum);

FloatTensorRW * outputMsg_moveFloatTensor(
        DataBlock *block, int outPortNum);

BranchRW * outputMsg_moveBranch(
        DataBlock *block, int outPortNum);

IntTensorR * contData_getIntTensor(
        DataBlock *block, int contDataNum);

FloatTensorR * contData_getFloatTensor(
        DataBlock *block, int contDataNum);

BranchR * contData_getBranch(
        DataBlock *block, int outPortNum);

IntTensorRW * contData_setIntTensor(
        DataBlock *block, int contDataNum, NumSizes numSizesV, ...);

FloatTensorRW * contData_setFloatTensor(
        DataBlock *block, int contDataNum, NumSizes numSizesV, ...);

BranchRW * contData_setBranch(
        DataBlock *block, int outPortNum, int size);

IntTensorRW * contData_writeIntTensor(
        DataBlock *block, int contDataNum, NumSizes numSizesV, ...);

FloatTensorRW * contData_writeFloatTensor(
        DataBlock *block, int contDataNum, NumSizes numSizesV, ...);

BranchRW * contData_writeBranch(
        DataBlock *block, int outPortNum, int size);

#define INDEX(sizes, indices...) \
    __INDEX_DISPATCH(indices, \
        __INDEX_8, __INDEX_7, __INDEX_6, __INDEX_5, \
        __INDEX_4, __INDEX_3, __INDEX_2, __INDEX_1)(sizes, indices)

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