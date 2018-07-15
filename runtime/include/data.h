#ifndef DATA_H
#define DATA_H

#define APPEND_INDEX -1

typedef struct NumSizes {
    int value;
} NumSizes;

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

#endif