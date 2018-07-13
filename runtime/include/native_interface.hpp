#ifndef NATIVE_INTERFACE_HPP
#define NATIVE_INTERFACE_HPP

#include <utility>

#include "data.hpp"

extern "C" {
#include "native_interface.h"
}

class DataBlock final {
private:
    DataPtr inputMsg_;
    std::vector<DataPtr> outputMsgs_;

    friend IntTensorR * inputMsg_getIntTensor(
            DataBlock *block);

    friend FloatTensorR * inputMsg_getFloatTensor(
            DataBlock *block);

    friend BranchR * inputMsg_getBranch(DataBlock *block);

    friend IntTensorRW * outputMsg_makeIntTensor(
        DataBlock *block, int outPortNum, NumSizes numSizesV, ...);

    friend FloatTensorRW * outputMsg_makeFloatTensor(
        DataBlock *block, int outPortNum, NumSizes numSizesV, ...);

    friend BranchRW * outputMsg_makeBranch(
        DataBlock *block, int outPortNum, int size);

    friend IntTensorRW * outputMsg_moveIntTensor(
        DataBlock *block, int outPortNum);

    friend FloatTensorRW * outputMsg_moveFloatTensor(
        DataBlock *block, int outPortNum);

    friend BranchRW * outputMsg_moveBranch(
        DataBlock *block, int outPortNum);

public:
    DataBlock(int numOutputs);

    int numOutputs() {
        return outputMsgs_.size();
    }

    void setInputMsg(DataPtr &&data) noexcept {
        inputMsg_ = std::move(data);
    }

    DataPtr && takeOutputMsg(int outPortNum) noexcept {
        return std::move(outputMsgs_[outPortNum]);
    }
};

#endif