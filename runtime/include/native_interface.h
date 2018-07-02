#ifndef NATIVE_INTERFACE_H
#define NATIVE_INTERFACE_H

#define APPEND_INDEX -1

static void staticAsserts() {
    (void)staticAsserts;
    switch(0) {
        case 0: break;
        case (sizeof(int) == 4? 1 : 0): return;
        case (sizeof(float) == 4? 2 : 0): return;
    }
}

/*** Type definitions ***/

typedef struct NumSizes {
    int value;
} NumSizes;

typedef struct DataBlock DataBlock;

typedef struct IntTensorR {
    int const numSizes;
    int const * const sizes;
    int const * const contents;
} IntTensorR;

typedef struct IntTensorRW {
    int const numSizes;
    int const * const sizes;
    int * const contents;
} IntTensorRW;

typedef struct IntTensorM {
    int numSizes;
    int *sizes;
    int *contents;
} IntTensorM;

typedef struct FloatTensorR {
    int const numSizes;
    int const * const sizes;
    float const * const contents;
} FloatTensorR;

typedef struct FloatTensorRW {
    int const numSizes;
    int const * const sizes;
    float * const contents;
} FloatTensorRW;

typedef struct FloatTensorM {
    int numSizes;
    int *sizes;
    float *contents;
} FloatTensorM;

typedef struct BranchR BranchR;

typedef struct BranchRW BranchRW;


/*** System functions ***/

int input_getNum(DataBlock *block);

IntTensorR * input_getIntTensor(DataBlock *block, int inPortNum);

FloatTensorR * input_getFloatTensor(DataBlock *block, int inPortNum);

BranchR * input_getBranch(DataBlock *block, int inPortNum);

int output_getNum(DataBlock *block);

IntTensorRW * output_makeIntTensor(
        DataBlock *block, int outPortNum, NumSizes numSizesV, ...);

FloatTensorRW * output_makeFloatTensor(
        DataBlock *block, int outPortNum, NumSizes numSizesV, ...);

BranchRW * output_makeBranch(
        DataBlock *block, int outPortNum, int size);

IntTensorRW * output_moveIntTensor(
        DataBlock *block, int outPortNum, int inPortNum);

FloatTensorRW * output_moveFloatTensor(
        DataBlock *block, int outPortNum, int inPortNum);

BranchRW * output_moveBranch(
        DataBlock *block, int outPortNum, int inPortNum);

int branchR_getSize(BranchR *branch);

int branchRW_getSize(BranchRW *branch);

IntTensorR * branchR_getIntTensor(BranchR *branch, int index);

IntTensorRW * branchRW_getIntTensor(BranchRW *branch, int index);

FloatTensorR * branchR_getFloatTensor(BranchR *branch, int index);

FloatTensorRW * branchRW_getFloatTensor(BranchRW *branch, int index);

BranchR * branchR_getBranch(BranchR *branch, int index);

BranchRW * branchRW_getBranch(BranchRW *branch, int index);

IntTensorRW * branchRW_makeIntTensor(
        BranchRW *branch, int indexInclAppend, NumSizes numSizesV, ...);

FloatTensorRW * branchRW_makeFloatTensor(
        BranchRW *branch, int indexInclAppend, NumSizes numSizesV, ...);

BranchRW * branchRW_makeBranch(
        BranchRW *branch, int indexInclAppend, int size);

void branchRW_pop(BranchRW *branch);

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