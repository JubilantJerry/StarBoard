#ifndef NATIVE_INTERFACE_HPP
#define NATIVE_INTERFACE_HPP

#include <utility>
#include <vector>
#include <unordered_set>
#include <boost/pool/pool_alloc.hpp>

#include "data.hpp"
#include "data_reference.hpp"

extern "C" {
#include "data_interface.h"
}

class DataBlock final {
private:
    DataPtr inputMsg_;
    std::vector<DataPtr> outputMsgs_;
    std::vector<DataReference> contData_;
    std::unordered_set<int> contDataWritten_;

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

    friend IntTensorR * contData_getIntTensor(
        DataBlock *block, int contDataNum);

    friend FloatTensorR * contData_getFloatTensor(
        DataBlock *block, int contDataNum);

    friend BranchR * contData_getBranch(
        DataBlock *block, int outPortNum);

    friend IntTensorRW * contData_setIntTensor(
        DataBlock *block, int contDataNum, NumSizes numSizesV, ...);

    friend FloatTensorRW * contData_setFloatTensor(
        DataBlock *block, int contDataNum, NumSizes numSizesV, ...);

    friend BranchRW * contData_setBranch(
        DataBlock *block, int outPortNum, int size);

    friend IntTensorRW * contData_writeIntTensor(
        DataBlock *block, int contDataNum, NumSizes numSizesV, ...);

    friend FloatTensorRW * contData_writeFloatTensor(
        DataBlock *block, int contDataNum, NumSizes numSizesV, ...);

    friend BranchRW * contData_writeBranch(
        DataBlock *block, int outPortNum, int size);

public:
    DataBlock(): DataBlock(0, 0) {}

    DataBlock(int numOutputs, int numContData);

    int numOutputs() {
        return outputMsgs_.size();
    }

    void setInputMsg(DataPtr &&data) noexcept {
        inputMsg_ = std::move(data);
    }

    DataPtr && takeOutputMsg(int outPortNum) noexcept {
        return std::move(outputMsgs_[outPortNum]);
    }

    DataSharedPtr getContData(int contDataNum) noexcept {
        return contData_[contDataNum].readAcquire();
    }

    void finalize() {
        using IterT = std::unordered_set<int>::iterator;

        IterT end = contDataWritten_.end();

        for (IterT it = contDataWritten_.begin(); it != end; it++) {
            contData_[*it].writeFinalize();
        }
        contDataWritten_.clear();
    }
};

#endif