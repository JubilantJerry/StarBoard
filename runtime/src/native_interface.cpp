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

    payload_.m.numSizes = numSizesV.value;
    payload_.m.sizes = new int[payload_.m.numSizes];
    int totalSize = fillSizes(
        payload_.m.numSizes, sizesList, payload_.m.sizes);
    payload_.m.contents = new int[totalSize];
}

IntTensorObj::IntTensorObj(NumSizes numSizesV, ...)
        : IntTensorObj() {

    va_list sizesList;
    va_start(sizesList, numSizesV);
    fillSelf(numSizesV, sizesList);
    va_end(sizesList);
}

IntTensorObj::IntTensorObj(NumSizes numSizesV, va_list sizesList)
        : IntTensorObj() {

    fillSelf(numSizesV, sizesList);
}

std::ostream& IntTensorObj::print(std::ostream &stream) const {
    stream << "IntTensorObj";
    stream << std::make_pair(
        payload_.m.sizes, payload_.m.sizes + payload_.m.numSizes);
    return stream;
}



inline void FloatTensorObj::fillSelf(
        NumSizes numSizesV, va_list sizesList) {

    payload_.m.numSizes = numSizesV.value;
    payload_.m.sizes = new int[payload_.m.numSizes];
    int totalSize = fillSizes(
        payload_.m.numSizes, sizesList, payload_.m.sizes);
    payload_.m.contents = new float[totalSize];
}

FloatTensorObj::FloatTensorObj(NumSizes numSizesV, ...)
        : FloatTensorObj() {

    va_list sizesList;
    va_start(sizesList, numSizesV);
    fillSelf(numSizesV, sizesList);
    va_end(sizesList);
}

FloatTensorObj::FloatTensorObj(NumSizes numSizesV, va_list sizesList)
        : FloatTensorObj() {

    fillSelf(numSizesV, sizesList);
}

std::ostream& FloatTensorObj::print(std::ostream &stream) const {
    stream << "FloatTensorObj";
    stream << std::make_pair(
        payload_.m.sizes, payload_.m.sizes + payload_.m.numSizes);
    return stream;
}



std::ostream& BranchObj::print(std::ostream &stream) const {
    stream << values_;
    return stream;
}



DataBlock::DataBlock(int numOutputs)
    :inputMsg_(), outputMsgs_(numOutputs) {}



static inline IntTensorObj * asIntTensor(DataPtr &data) {
    return static_cast<IntTensorObj *>(data.get());
}

static inline FloatTensorObj * asFloatTensor(DataPtr &data) {
    return static_cast<FloatTensorObj *>(data.get());
}

static inline BranchObj * asBranch(DataPtr &data) {
    return static_cast<BranchObj *>(data.get());
}

static inline BranchObj * accessBranchR(BranchR *branch) {
    return static_cast<BranchObj *>(branch);
}

static inline BranchObj * accessBranchRW(BranchRW *branch) {
    return static_cast<BranchObj *>(branch);
}

IntTensorR * inputMsg_getIntTensor(DataBlock *block) {
    return &asIntTensor(block->inputMsg_)->getR();
}

FloatTensorR * inputMsg_getFloatTensor(DataBlock *block) {
    return &asFloatTensor(block->inputMsg_)->getR();
}

BranchR * inputMsg_getBranch(DataBlock *block) {
    return asBranch(block->inputMsg_);
}

IntTensorRW * outputMsg_makeIntTensor(
        DataBlock *block, int outPortNum, NumSizes numSizesV, ...) {

    va_list sizesList;
    va_start(sizesList, numSizesV);
    DataPtr &output = block->outputMsgs_[outPortNum];
    output = make_unique<IntTensorObj>(numSizesV, sizesList);
    va_end(sizesList);
    return &asIntTensor(output)->getRW();
}

FloatTensorRW * outputMsg_makeFloatTensor(
        DataBlock *block, int outPortNum, NumSizes numSizesV, ...) {

    va_list sizesList;
    va_start(sizesList, numSizesV);
    DataPtr &output = block->outputMsgs_[outPortNum];
    output = make_unique<FloatTensorObj>(numSizesV, sizesList);
    va_end(sizesList);
    return &asFloatTensor(output)->getRW();
}

BranchRW * outputMsg_makeBranch(
        DataBlock *block, int outPortNum, int size) {

    DataPtr &output = block->outputMsgs_[outPortNum];
    output = make_unique<BranchObj>(size);
    return asBranch(output);
}

IntTensorRW * outputMsg_moveIntTensor(
        DataBlock *block, int outPortNum) {

    DataPtr &output = block->outputMsgs_[outPortNum];
    output = std::move(block->inputMsg_);
    return &asIntTensor(output)->getRW();
}

FloatTensorRW * outputMsg_moveFloatTensor(
        DataBlock *block, int outPortNum) {

    DataPtr &output = block->outputMsgs_[outPortNum];
    output = std::move(block->inputMsg_);
    return &asFloatTensor(output)->getRW();
}

BranchRW * outputMsg_moveBranch(
        DataBlock *block, int outPortNum) {

    DataPtr &output = block->outputMsgs_[outPortNum];
    output = std::move(block->inputMsg_);
    return asBranch(output);
}

int branchR_getSize(BranchR *branch) {
    return static_cast<BranchObj *>(branch)->size();
}

int branchRW_getSize(BranchRW *branch) {
    return static_cast<BranchObj *>(branch)->size();
}

IntTensorR * branchR_getIntTensor(BranchR *branch, int index) {
    return &asIntTensor(accessBranchR(branch)->values_[index])->getR();
}

IntTensorRW * branchRW_getIntTensor(BranchRW *branch, int index) {
    return &asIntTensor(accessBranchRW(branch)->values_[index])->getRW();
}

FloatTensorR * branchR_getFloatTensor(BranchR *branch, int index) {
    return &asFloatTensor(accessBranchR(branch)->values_[index])->getR();
}

FloatTensorRW * branchRW_getFloatTensor(BranchRW *branch, int index) {
    return &asFloatTensor(accessBranchRW(branch)->values_[index])->getRW();
}

BranchR * branchR_getBranch(BranchR *branch, int index) {
    return asBranch(accessBranchR(branch)->values_[index]);
}

BranchRW * branchRW_getBranch(BranchRW *branch, int index) {
    return asBranch(accessBranchRW(branch)->values_[index]);
}

IntTensorRW * branchRW_makeIntTensor(
        BranchRW *branch, int indexInclAppend, NumSizes numSizesV, ...) {

    BranchObj *branchObj = accessBranchRW(branch);
    IntTensorRW *result;

    va_list sizesList;
    va_start(sizesList, numSizesV);
    DataPtr data = make_unique<IntTensorObj>(numSizesV, sizesList);
    if (indexInclAppend == APPEND_INDEX) {
        branchObj->values_.push_back(std::move(data));
        result = &asIntTensor(branchObj->values_.back())->getRW();
    } else {
        DataPtr &value = branchObj->values_[indexInclAppend];
        value = std::move(data);
        result = &asIntTensor(value)->getRW();
    }
    va_end(sizesList);
    return result;
}

FloatTensorRW * branchRW_makeFloatTensor(
        BranchRW *branch, int indexInclAppend, NumSizes numSizesV, ...) {

    BranchObj *branchObj = accessBranchRW(branch);
    FloatTensorRW *result;

    va_list sizesList;
    va_start(sizesList, numSizesV);
    DataPtr data = make_unique<IntTensorObj>(numSizesV, sizesList);
    if (indexInclAppend == APPEND_INDEX) {
        branchObj->values_.push_back(std::move(data));
        result = &asFloatTensor(branchObj->values_.back())->getRW();
    } else {
        DataPtr &value = branchObj->values_[indexInclAppend];
        value = std::move(data);
        result = &asFloatTensor(value)->getRW();
    }
    va_end(sizesList);
    return result;
}

BranchRW * branchRW_makeBranch(
        BranchRW *branch, int indexInclAppend, int size) {

    BranchObj *branchObj = accessBranchRW(branch);
    BranchRW *result;

    DataPtr data = make_unique<BranchObj>(size);
    if (indexInclAppend == APPEND_INDEX) {
        branchObj->values_.push_back(std::move(data));
        result = asBranch(branchObj->values_.back());
    } else {
        DataPtr &value = branchObj->values_[indexInclAppend];
        value = std::move(data);
        result = asBranch(value);
    }
    return result;
}

void branchRW_pop(BranchRW *branch) {
    accessBranchRW(branch)->values_.pop_back();
}