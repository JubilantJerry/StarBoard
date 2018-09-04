#include <custom_utility.hpp>

#include "data_interface.hpp"

DataBlock::DataBlock(int numOutputs, int numContData)
    :inputMsg_(), outputMsgs_(numOutputs), contData_(numContData) {}

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
    output = make_unique<Branch>(size);
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

IntTensorR * contData_getIntTensor(DataBlock *block, int contDataNum) {
    return &asIntTensor(block->contData_[contDataNum]->readAcquire())->getR();
}

FloatTensorR * contData_getFloatTensor(DataBlock *block, int contDataNum) {
    return &asFloatTensor(block->contData_[contDataNum]->readAcquire())->getR();
}

BranchR * contData_getBranch(DataBlock *block, int contDataNum) {
    return asBranch(block->contData_[contDataNum]->readAcquire());
}

IntTensorRW * contData_setIntTensor(
        DataBlock *block, int contDataNum, NumSizes numSizesV, ...) {

    block->contDataWritten_.insert(contDataNum);

    va_list sizesList;
    va_start(sizesList, numSizesV);
    DataPtr output = make_unique<IntTensor>(numSizesV, sizesList);
    va_end(sizesList);

    return &asIntTensor(block->contData_[contDataNum]->setWriteAcquire(
        std::move(output)))->getRW();
}

FloatTensorRW * contData_setFloatTensor(
        DataBlock *block, int contDataNum, NumSizes numSizesV, ...) {

    block->contDataWritten_.insert(contDataNum);

    va_list sizesList;
    va_start(sizesList, numSizesV);
    DataPtr output = make_unique<FloatTensor>(numSizesV, sizesList);
    va_end(sizesList);
    return &asFloatTensor(block->contData_[contDataNum]->setWriteAcquire(
        std::move(output)))->getRW();
}

BranchRW * contData_setBranch(
        DataBlock *block, int contDataNum, int size) {

    block->contDataWritten_.insert(contDataNum);

    DataPtr output = make_unique<Branch>(size);
    return asBranch(block->contData_[contDataNum]->setWriteAcquire(
        std::move(output)));
}

IntTensorRW * contData_writeIntTensor(
        DataBlock *block, int contDataNum, NumSizes numSizesV, ...) {

    block->contDataWritten_.insert(contDataNum);
    DataReference *dataRef = block->contData_[contDataNum];

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

FloatTensorRW * contData_writeFloatTensor(
        DataBlock *block, int contDataNum, NumSizes numSizesV, ...) {

    block->contDataWritten_.insert(contDataNum);
    DataReference *dataRef = block->contData_[contDataNum];

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

BranchRW * contData_writeBranch(
        DataBlock *block, int contDataNum, int size) {

    block->contDataWritten_.insert(contDataNum);
    DataReference *dataRef = block->contData_[contDataNum];

    if (dataRef->canDirectWriteAcquire()) {
        return asBranch(dataRef->directWriteAcquire());
    } else {
        DataPtr output = make_unique<Branch>(size);
        return asBranch(dataRef->setWriteAcquire(std::move(output)));
    }
}