#ifndef DATA_HPP
#define DATA_HPP

#include <iostream>
#include <memory>
#include <vector>
#include <cstdarg>

extern "C" {
#include "data.h"
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

class IntTensor final: public Data {
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
    IntTensor() noexcept {};
    IntTensor(NumSizes numSizesV, ...);
    IntTensor(NumSizes numSizesV, va_list args);

    ~IntTensor() noexcept {
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

class FloatTensor final: public Data {
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
    FloatTensor() noexcept {};
    FloatTensor(NumSizes numSizesV, ...);
    FloatTensor(NumSizes numSizesV, va_list args);

    ~FloatTensor() noexcept {
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

    friend IntTensorR * branchR_getIntTensor(
            BranchR *branch, int index);

    friend IntTensorRW * branchRW_getIntTensor(
            BranchRW *branch, int index);

    friend FloatTensorR * branchR_getFloatTensor(
            BranchR *branch, int index);

    friend FloatTensorRW * branchRW_getFloatTensor(
            BranchRW *branch, int index);

    friend BranchR * branchR_getBranch(
            BranchR *branch, int index);

    friend BranchRW * branchRW_getBranch(
            BranchRW *branch, int index);

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


#endif