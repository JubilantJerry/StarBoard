#include <custom_utility.hpp>

#include "data_interface.hpp"

DataBlock::DataBlock(int numOutputs)
    :inputMsg_(), outputMsgs_(numOutputs) {}

static inline IntTensor * asIntTensor(DataPtr &data) {
    return static_cast<IntTensor *>(data.get());
}

static inline FloatTensor * asFloatTensor(DataPtr &data) {
    return static_cast<FloatTensor *>(data.get());
}

static inline BranchObj * asBranch(DataPtr &data) {
    return static_cast<BranchObj *>(data.get());
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
    output = make_unique<IntTensor>(numSizesV, sizesList);
    va_end(sizesList);
    return &asIntTensor(output)->getRW();
}

FloatTensorRW * outputMsg_makeFloatTensor(
        DataBlock *block, int outPortNum, NumSizes numSizesV, ...) {

    va_list sizesList;
    va_start(sizesList, numSizesV);
    DataPtr &output = block->outputMsgs_[outPortNum];
    output = make_unique<FloatTensor>(numSizesV, sizesList);
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