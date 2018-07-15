#ifndef DATA_HPP
#define DATA_HPP

#include <iostream>
#include <memory>
#include <vector>
#include <cstdarg>

extern "C" {
#include "data.h"
}

class Data;
class DataVisitor;

using DataPtr = std::unique_ptr<Data>;

class Data {
protected:
    virtual void print(std::ostream &stream) const = 0;

    virtual void requestVisit(DataVisitor &visitor) const = 0;

public:
    Data() noexcept = default;
    virtual ~Data() noexcept = default;
    Data(Data const&) = delete;
    Data(Data&&) noexcept = default;
    Data& operator=(Data const&) = delete;
    virtual Data& operator=(Data&&) noexcept = default;

    friend std::ostream& operator<<(std::ostream &stream, Data const &data) {
        data.print(stream);
        return stream;
    }

    friend DataVisitor& operator&(DataVisitor &visitor, Data const &data) {
        data.requestVisit(visitor);
        return visitor;
    }

    friend DataVisitor&& operator&(DataVisitor &&visitor, Data const &data) {
        data.requestVisit(visitor);
        return std::move(visitor);
    }
};

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

    virtual void print(std::ostream &stream) const override;
    virtual void requestVisit(DataVisitor &visitor) const override;

public:
    IntTensor() noexcept {};
    IntTensor(NumSizes numSizesV, ...);
    IntTensor(NumSizes numSizesV, va_list args);
    IntTensor(IntTensorM &&payload) {
        payload_.m = payload;
    }

    ~IntTensor() noexcept {
        delete[] payload_.m.sizes;
        delete[] payload_.m.contents;
    }

    int numSizes() const noexcept {
        return payload_.m.numSizes;
    }

    int const *sizes() const noexcept {
        return payload_.m.sizes;
    }

    int *contents() noexcept {
        return payload_.m.contents;
    }

    int const *contents() const noexcept {
        return payload_.m.contents;
    }

    IntTensorR &getR() noexcept {
        return payload_.r;
    }

    IntTensorRW &getRW() noexcept {
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

    virtual void print(std::ostream &stream) const override;
    virtual void requestVisit(DataVisitor &visitor) const override;

public:
    FloatTensor() noexcept {};
    FloatTensor(NumSizes numSizesV, ...);
    FloatTensor(NumSizes numSizesV, va_list args);
    FloatTensor(FloatTensorM &&payload) {
        payload_.m = payload;
    }

    ~FloatTensor() noexcept {
        delete[] payload_.m.sizes;
        delete[] payload_.m.contents;
    }

    int numSizes() const noexcept {
        return payload_.m.numSizes;
    }

    int const *sizes() const noexcept {
        return payload_.m.sizes;
    }

    float *contents() noexcept {
        return payload_.m.contents;
    }

    float const *contents() const noexcept {
        return payload_.m.contents;
    }

    FloatTensorR &getR() noexcept {
        return payload_.r;
    }

    FloatTensorRW &getRW() noexcept {
        return payload_.rw;
    }
};

struct BranchR {};

struct BranchRW {};

class Branch final: public BranchR, public BranchRW, public Data {
private:
    std::vector<DataPtr> values_;

    virtual void print(std::ostream &stream) const override;
    virtual void requestVisit(DataVisitor &visitor) const override;

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
    Branch(int size): values_(size) {}

    DataPtr const & getValue(int index) const noexcept {
        return values_[index];
    }

    void setValue(int index, DataPtr &&data) noexcept {
        values_[index] = std::move(data);
    }

    int size() const noexcept {
        return values_.size();
    }
};

class DataVisitor {
public:
    virtual void visitIntTensor(IntTensor const &data) = 0;
    virtual void visitFloatTensor(FloatTensor const &data) = 0;
    virtual void visitBranch(Branch const &data) = 0;
};

#endif