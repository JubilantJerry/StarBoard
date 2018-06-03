#ifndef NATIVE_INTERFACE_HPP
#define NATIVE_INTERFACE_HPP

#include <iostream>
#include <utility>
#include <memory>
#include <vector>
#include <cstdarg>

#include "custom_utility.hpp"

extern "C" {
#include "native_interface.h"
}

class Data {
private:
    friend std::ostream& operator<<(std::ostream &stream, const Data &data) {
        return data.print(stream);
    }

protected:
    virtual std::ostream& print(std::ostream &stream) const = 0;

public:
    Data() noexcept = default;
    virtual ~Data() noexcept = default;
    Data(Data const&) = delete;
    Data(Data&&) noexcept = default;
    Data& operator=(Data const&) = delete;
    virtual Data& operator=(Data&&) noexcept = default;
};

using DataPtr = std::unique_ptr<Data>;

class IntTensorObj final: public IntTensorStruct, public Data {
private:
    using IntTensorStruct::numSizes;
    using IntTensorStruct::sizes;
    using IntTensorStruct::contents;

    void fillSelf(NumSizes numSizesV, va_list args);

    std::ostream& print(std::ostream &stream) const override;

public:
    IntTensorObj() noexcept {
        numSizes = 0;
        sizes = nullptr;
        contents = nullptr;
    };
    IntTensorObj(NumSizes numSizesV, ...);
    IntTensorObj(NumSizes numSizesV, va_list args);

    ~IntTensorObj() noexcept {
        delete[] sizes;
        delete[] contents;
    }

    int getNumSizes() {
        return numSizes;
    }

    int *getSizes() {
        return sizes;
    }

    int *getContents() {
        return contents;
    }
};

class FloatTensorObj final: public FloatTensorStruct, public Data {
private:
    using FloatTensorStruct::numSizes;
    using FloatTensorStruct::sizes;
    using FloatTensorStruct::contents;

    void fillSelf(NumSizes numSizesV, va_list args);

    std::ostream& print(std::ostream &stream) const override;

public:
    FloatTensorObj() noexcept {
        numSizes = 0;
        sizes = nullptr;
        contents = nullptr;
    };

    FloatTensorObj(NumSizes numSizesV, ...);
    FloatTensorObj(NumSizes numSizesV, va_list sizesList);

    ~FloatTensorObj() noexcept {
        delete[] sizes;
        delete[] contents;
    }

    int getNumSizes() noexcept {
        return numSizes;
    }

    int *getSizes() noexcept {
        return sizes;
    }

    float *getContents() noexcept {
        return contents;
    }
};

class Branch final: public Data {
private:
    std::vector<DataPtr> values;

    std::ostream& print(std::ostream &stream) const override;

    friend int getBranchSize(Branch const *branch) {
        return branch->getSize();
    }

    friend IntTensorStruct const * getBranchIntTensor(
            Branch const *branch, int index) {

        return static_cast<IntTensorObj const *>(
            branch->values[index].get());
    }

    friend FloatTensorStruct const * getBranchFloatTensor(
            Branch const *branch, int index) {

        return static_cast<FloatTensorObj const *>(
            branch->values[index].get());
    }

    friend Branch const * getBranchBranch(
            Branch const *branch, int index) {

        return static_cast<Branch const *>(
            branch->values[index].get());
    }


public:
    Branch():  values(0) {}

    Branch(int size): values(size) {}

    void setValue(int index, DataPtr &&data) noexcept {
        values[index] = std::move(data);
    }

    int getSize() const noexcept {
        return values.size();
    }
};

class DataBlock final {
private:
    std::vector<DataPtr> inputs;
    std::vector<DataPtr> outputs;

    friend int getNumInputs(DataBlock const *block) {
        return block->getNumInputs();
    }

    friend IntTensorStruct const * getInputIntTensor(
            DataBlock const *block, int inPortNum) {
        return static_cast<IntTensorObj const *>(
            block->inputs[inPortNum].get());
    }

    friend FloatTensorStruct const * getInputFloatTensor(
            DataBlock const *block, int inPortNum) {
        return static_cast<FloatTensorObj const *>(
            block->inputs[inPortNum].get());
    }

    friend Branch const * getInputBranch(
            DataBlock const *block, int inPortNum) {
        return static_cast<Branch const *>(
            block->inputs[inPortNum].get());
    }

    friend int getNumOutputs(DataBlock const *block) {
        return block->getNumOutputs();
    }

    friend IntTensorStruct const * makeOutputIntTensor(
            DataBlock *block, int outPortNum,
            NumSizes numSizesV, ...) {
        va_list sizesList;
        va_start(sizesList, numSizesV);
        block->outputs[outPortNum] =
            make_unique<IntTensorObj>(numSizesV, sizesList);
        va_end(sizesList);
        return static_cast<IntTensorObj const *>(
            block->outputs[outPortNum].get());
    }

    friend FloatTensorStruct const * makeOutputFloatTensor(
            DataBlock *block, int outPortNum,
            NumSizes numSizesV, ...) {
        va_list sizesList;
        va_start(sizesList, numSizesV);
        block->outputs[outPortNum] =
            make_unique<FloatTensorObj>(numSizesV, sizesList);
        va_end(sizesList);
        return static_cast<FloatTensorObj const *>(
            block->outputs[outPortNum].get());
    }

    friend Branch const * makeOutputBranch(
            DataBlock *block, int outPortNum, int size) {
        block->outputs[outPortNum] = make_unique<Branch>(size);
        return static_cast<Branch const *>(
            block->outputs[outPortNum].get());
    }

    friend IntTensorStruct const * moveToOutputIntTensor(
            DataBlock *block, int outPortNum, int inPortNum) {
        block->outputs[outPortNum] = std::move(block->inputs[inPortNum]);
        return static_cast<IntTensorObj const *>(
            block->outputs[outPortNum].get());
    }

    friend FloatTensorStruct const * moveToOutputFloatTensor(
            DataBlock *block, int outPortNum, int inPortNum) {
        block->outputs[outPortNum] = std::move(block->inputs[inPortNum]);
        return static_cast<FloatTensorObj const *>(
            block->outputs[outPortNum].get());
    }

    friend Branch const * moveToOutputBranch(
            DataBlock *block, int outPortNum, int inPortNum) {
        block->outputs[outPortNum] = std::move(block->inputs[inPortNum]);
        return static_cast<Branch const *>(
            block->outputs[outPortNum].get());
    }

public:
    DataBlock(int numInputs, int numStateData, int numOutputs);

    int getNumInputs() const noexcept {
        return inputs.size();
    }

    int getNumOutputs() const noexcept {
        return outputs.size();
    }

    void setInput(int inPortNum, DataPtr &&data) noexcept {
        inputs[inPortNum] = std::move(data);
    }

    DataPtr && takeOutput(int outPortNum) noexcept {
        return std::move(outputs[outPortNum]);
    }
};

#endif