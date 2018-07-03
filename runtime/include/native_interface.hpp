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

class IntTensorObj final: public Data {
private:
    union PayloadUnion {
        IntTensorR r;
        IntTensorRW rw;
        IntTensorM m;

        PayloadUnion() {
            m.numSizes = 0;
            m.sizes = nullptr;
            m.contents = nullptr;
        }
    } payload_;

    void fillSelf(NumSizes numSizesV, va_list args);

    std::ostream& print(std::ostream &stream) const override;

public:
    IntTensorObj() noexcept {};
    IntTensorObj(NumSizes numSizesV, ...);
    IntTensorObj(NumSizes numSizesV, va_list args);

    ~IntTensorObj() noexcept {
        delete[] payload_.m.sizes;
        delete[] payload_.m.contents;
    }

    int numSizes() {
        return payload_.m.numSizes;
    }

    int const *sizes() {
        return payload_.m.sizes;
    }

    int *contents() {
        return payload_.m.contents;
    }

    IntTensorR &getR() {
        return payload_.r;
    }

    IntTensorRW &getRW() {
        return payload_.rw;
    }
};

class FloatTensorObj final: public Data {
private:
    union PayloadUnion {
        FloatTensorR r;
        FloatTensorRW rw;
        FloatTensorM m;

        PayloadUnion() {
            m.numSizes = 0;
            m.sizes = nullptr;
            m.contents = nullptr;
        }
    } payload_;

    void fillSelf(NumSizes numSizesV, va_list args);

    std::ostream& print(std::ostream &stream) const override;

public:
    FloatTensorObj() noexcept {};
    FloatTensorObj(NumSizes numSizesV, ...);
    FloatTensorObj(NumSizes numSizesV, va_list args);

    ~FloatTensorObj() noexcept {
        delete[] payload_.m.sizes;
        delete[] payload_.m.contents;
    }

    int numSizes() {
        return payload_.m.numSizes;
    }

    int const *sizes() {
        return payload_.m.sizes;
    }

    float *contents() {
        return payload_.m.contents;
    }

    FloatTensorR &getR() {
        return payload_.r;
    }

    FloatTensorRW &getRW() {
        return payload_.rw;
    }
};

struct BranchR {};

struct BranchRW {};

class BranchObj final: public BranchR, public BranchRW, public Data {
private:
    std::vector<DataPtr> values_;

    std::ostream& print(std::ostream &stream) const override;

    friend IntTensorR * branchR_getIntTensor(BranchR *branch, int index);

    friend IntTensorRW * branchRW_getIntTensor(BranchRW *branch, int index);

    friend FloatTensorR * branchR_getFloatTensor(BranchR *branch, int index);

    friend FloatTensorRW * branchRW_getFloatTensor(BranchRW *branch, int index);

    friend BranchR * branchR_getBranch(BranchR *branch, int index);

    friend BranchRW * branchRW_getBranch(BranchRW *branch, int index);

    friend IntTensorRW * branchRW_makeIntTensor(
            BranchRW *branch, int indexInclAppend, NumSizes numSizesV, ...);

    friend FloatTensorRW * branchRW_makeFloatTensor(
            BranchRW *branch, int indexInclAppend, NumSizes numSizesV, ...);

    friend BranchRW * branchRW_makeBranch(
            BranchRW *branch, int indexInclAppend, int size);

    friend void branchRW_pop(BranchRW *branch);

public:
    BranchObj(int size): values_(size) {}

    void setValue(int index, DataPtr &&data) noexcept {
        values_[index] = std::move(data);
    }

    int size() const noexcept {
        return values_.size();
    }
};




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

    void setInputMsg(DataPtr &&data) noexcept {
        inputMsg_ = std::move(data);
    }

    DataPtr && takeOutputMsg(int outPortNum) noexcept {
        return std::move(outputMsgs_[outPortNum]);
    }
};

#endif