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

using IntSet = std::unordered_set<
    int,
    std::hash<int>,
    std::equal_to<int>,
    boost::fast_pool_allocator<int>
>;

class DataBlock final {
private:
    DataPtr inputMsg_;
    std::vector<DataPtr> outputMsgs_;
    IntSet outputMsgWritten_;
    std::vector<DataReference const *> inputContData_;
    std::vector<DataReference *> outputContData_;
    IntSet outputContDataWritten_;

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

    friend IntTensorR * inputContData_getIntTensor(
            DataBlock *block, int inContDataNum);

    friend FloatTensorR * inputContData_getFloatTensor(
            DataBlock *block, int inContDataNum);

    friend BranchR * inputContData_getBranch(
            DataBlock *block, int inContDataNum);

    friend IntTensorRW * outputContData_setIntTensor(
            DataBlock *block, int contDataNum, NumSizes numSizesV, ...);

    friend FloatTensorRW * outputContData_setFloatTensor(
            DataBlock *block, int contDataNum, NumSizes numSizesV, ...);

    friend BranchRW * outputContData_setBranch(
            DataBlock *block, int outPortNum, int size);

    friend IntTensorRW * outputContData_writeIntTensor(
            DataBlock *block, int contDataNum, NumSizes numSizesV, ...);

    friend FloatTensorRW * outputContData_writeFloatTensor(
            DataBlock *block, int contDataNum, NumSizes numSizesV, ...);

    friend BranchRW * outputContData_writeBranch(
            DataBlock *block, int outPortNum, int size);

public:
    DataBlock(int numOutputs = 0,
              int numInputContData = 0,
              int numOutputContData = 0);

    int numOutputs() const noexcept {
        return outputMsgs_.size();
    }

    void setInputMsg(DataPtr &&data) noexcept {
        inputMsg_ = std::move(data);
    }

    DataPtr && takeOutputMsg(int outPortNum) noexcept {
        return std::move(outputMsgs_[outPortNum]);
    }

    IntSet const & getOutputMsgWritten() const noexcept {
        return outputMsgWritten_;
    }

    void setInputContData(int inContDataNum, DataReference *dataRef) noexcept {
        inputContData_[inContDataNum] = dataRef;
    }

    void setOutputContData(int outContDataNum, DataReference *dataRef) noexcept {
        outputContData_[outContDataNum] = dataRef;
    }

    DataReference const * getInputContData(int inContDataNum) noexcept {
        return inputContData_[inContDataNum];
    }

    DataReference * getOutputContData(int outContDataNum) noexcept {
        return outputContData_[outContDataNum];
    }

    IntSet const & getOutputContDataWritten() const noexcept {
        return outputContDataWritten_;
    }

    void finalize();
};

#endif