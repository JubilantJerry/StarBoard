#include <iostream>
#include <utility>
#include <memory>
#include <vector>
#include <cstdarg>

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
    Data() = default;
    virtual ~Data() = default;
    Data(Data const&) = delete;
    Data(Data&&) = default;
    Data& operator=(Data const&) = delete;
    virtual Data& operator=(Data&&) = default;
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
    IntTensorObj() {
        numSizes = 0;
        sizes = nullptr;
        contents = nullptr;
    };
    IntTensorObj(NumSizes numSizesV, ...);
    IntTensorObj(NumSizes numSizesV, va_list args);

    ~IntTensorObj() {
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
    FloatTensorObj() {
        numSizes = 0;
        sizes = nullptr;
        contents = nullptr;
    };

    FloatTensorObj(NumSizes numSizesV, ...);
    FloatTensorObj(NumSizes numSizesV, va_list sizesList);

    ~FloatTensorObj() {
        delete[] sizes;
        delete[] contents;
    }

    int getNumSizes() {
        return numSizes;
    }

    int *getSizes() {
        return sizes;
    }

    float *getContents() {
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

    void setValue(int index, DataPtr &&data) {
        values[index] = std::move(data);
    }

    int getSize() const {
        return values.size();
    }
};

class DataBlock final {
private:
    std::vector<DataPtr> inputs;

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

public:
    DataBlock(int numInputs, int numStateData, int numOutputs);

    int getNumInputs() const {
        return inputs.size();
    }

    void setInput(int inPortNum, DataPtr &&data) {
        inputs[inPortNum] = std::move(data);
    }
};