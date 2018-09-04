#include <custom_utility.hpp>

#include "data_interface.hpp"

DataBlock::DataBlock(
        int numOutputs, int numInputContData, int numOutputContData)
    :inputMsg_(), outputMsgs_(numOutputs),
     inputContData_(numInputContData), outputContData_(numOutputContData) {}

void DataBlock::finalize() {
    IntSet::iterator end = outputContDataWritten_.end();

    for (IntSet::iterator it = outputContDataWritten_.begin();
         it != end; it++) {
        outputContData_[*it]->writeFinalize();
    }
}

template<typename DataPtrT>
static inline IntTensor * asIntTensor(DataPtrT &&data) {
    return static_cast<IntTensor *>(data.get());
}

template<typename DataPtrT>
static inline FloatTensor * asFloatTensor(DataPtrT &&data) {
    return static_cast<FloatTensor *>(data.get());
}

template<typename DataPtrT>
static inline Branch * asBranch(DataPtrT &&data) {
    return static_cast<Branch *>(data.get());
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

    block->outputMsgWritten_.insert(outPortNum);

    va_list sizesList;
    va_start(sizesList, numSizesV);
    DataPtr &output = block->outputMsgs_[outPortNum];
    output = make_unique<IntTensor>(numSizesV, sizesList);
    va_end(sizesList);
    return &asIntTensor(output)->getRW();
}

FloatTensorRW * outputMsg_makeFloatTensor(
        DataBlock *block, int outPortNum, NumSizes numSizesV, ...) {

    block->outputMsgWritten_.insert(outPortNum);

    va_list sizesList;
    va_start(sizesList, numSizesV);
    DataPtr &output = block->outputMsgs_[outPortNum];
    output = make_unique<FloatTensor>(numSizesV, sizesList);
    va_end(sizesList);
    return &asFloatTensor(output)->getRW();
}

BranchRW * outputMsg_makeBranch(
        DataBlock *block, int outPortNum, int size) {

    block->outputMsgWritten_.insert(outPortNum);

    DataPtr &output = block->outputMsgs_[outPortNum];
    output = make_unique<Branch>(size);
    return asBranch(output);
}

IntTensorRW * outputMsg_moveIntTensor(
        DataBlock *block, int outPortNum) {

    block->outputMsgWritten_.insert(outPortNum);

    DataPtr &output = block->outputMsgs_[outPortNum];
    output = std::move(block->inputMsg_);
    return &asIntTensor(output)->getRW();
}

FloatTensorRW * outputMsg_moveFloatTensor(
        DataBlock *block, int outPortNum) {

    block->outputMsgWritten_.insert(outPortNum);

    DataPtr &output = block->outputMsgs_[outPortNum];
    output = std::move(block->inputMsg_);
    return &asFloatTensor(output)->getRW();
}

BranchRW * outputMsg_moveBranch(
        DataBlock *block, int outPortNum) {

    block->outputMsgWritten_.insert(outPortNum);

    DataPtr &output = block->outputMsgs_[outPortNum];
    output = std::move(block->inputMsg_);
    return asBranch(output);
}

IntTensorR * inputContData_getIntTensor(
        DataBlock *block, int inContDataNum) {

    return &asIntTensor(
        block->inputContData_[inContDataNum]->readAcquire())->getR();
}

FloatTensorR * inputContData_getFloatTensor(
        DataBlock *block, int inContDataNum) {

    return &asFloatTensor(
        block->inputContData_[inContDataNum]->readAcquire())->getR();
}

BranchR * inputContData_getBranch(DataBlock *block, int inContDataNum) {
    return asBranch(block->inputContData_[inContDataNum]->readAcquire());
}

IntTensorRW * outputContData_setIntTensor(
        DataBlock *block, int outContDataNum, NumSizes numSizesV, ...) {

    block->outputContDataWritten_.insert(outContDataNum);

    va_list sizesList;
    va_start(sizesList, numSizesV);
    DataPtr output = make_unique<IntTensor>(numSizesV, sizesList);
    va_end(sizesList);

    return &asIntTensor(
        block->outputContData_[outContDataNum]->setWriteAcquire(
            std::move(output)
        )
    )->getRW();
}

FloatTensorRW * outputContData_setFloatTensor(
        DataBlock *block, int outContDataNum, NumSizes numSizesV, ...) {

    block->outputContDataWritten_.insert(outContDataNum);

    va_list sizesList;
    va_start(sizesList, numSizesV);
    DataPtr output = make_unique<FloatTensor>(numSizesV, sizesList);
    va_end(sizesList);
    return &asFloatTensor(
        block->outputContData_[outContDataNum]->setWriteAcquire(
            std::move(output)
        )
    )->getRW();
}

BranchRW * outputContData_setBranch(
        DataBlock *block, int outContDataNum, int size) {

    block->outputContDataWritten_.insert(outContDataNum);

    DataPtr output = make_unique<Branch>(size);
    return asBranch(
        block->outputContData_[outContDataNum]->setWriteAcquire(
            std::move(output)
        )
    );
}

IntTensorRW * outputContData_writeIntTensor(
        DataBlock *block, int outContDataNum, NumSizes numSizesV, ...) {

    block->outputContDataWritten_.insert(outContDataNum);
    DataReference *dataRef = block->outputContData_[outContDataNum];

    if (dataRef->canDirectWriteAcquire()) {
        return &asIntTensor(dataRef->directWriteAcquire())->getRW();
    } else {
        va_list sizesList;
        va_start(sizesList, numSizesV);
        DataPtr output = make_unique<IntTensor>(numSizesV, sizesList);
        va_end(sizesList);

        return &asIntTensor(dataRef->setWriteAcquire(
            std::move(output)))->getRW();
    }
}

FloatTensorRW * outputContData_writeFloatTensor(
        DataBlock *block, int outContDataNum, NumSizes numSizesV, ...) {

    block->outputContDataWritten_.insert(outContDataNum);
    DataReference *dataRef = block->outputContData_[outContDataNum];

    if (dataRef->canDirectWriteAcquire()) {
        return &asFloatTensor(dataRef->directWriteAcquire())->getRW();
    } else {
        va_list sizesList;
        va_start(sizesList, numSizesV);
        DataPtr output = make_unique<FloatTensor>(numSizesV, sizesList);
        va_end(sizesList);

        return &asFloatTensor(dataRef->setWriteAcquire(
            std::move(output)))->getRW();
    }
}

BranchRW * outputContData_writeBranch(
        DataBlock *block, int outContDataNum, int size) {

    block->outputContDataWritten_.insert(outContDataNum);
    DataReference *dataRef = block->outputContData_[outContDataNum];

    if (dataRef->canDirectWriteAcquire()) {
        return asBranch(dataRef->directWriteAcquire());
    } else {
        DataPtr output = make_unique<Branch>(size);
        return asBranch(dataRef->setWriteAcquire(std::move(output)));
    }
}