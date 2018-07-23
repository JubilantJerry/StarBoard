#include <utility>
#include <custom_utility.hpp>

#include "data.hpp"

static inline int fillSizes(int numSizes, va_list sizesList, int *sizes) {
    int totalSize = 1;
    for (int i = 0; i < numSizes; i++) {
        int size = va_arg(sizesList, int);
        sizes[i] = size;
        totalSize *= size;
    }
    return totalSize;
}

inline void IntTensor::fillSelf(
        NumSizes numSizesV, va_list sizesList) {

    payload_.m.numSizes = numSizesV.value;
    payload_.m.sizes = new int[payload_.m.numSizes];
    int totalSize = fillSizes(
        payload_.m.numSizes, sizesList, payload_.m.sizes);
    payload_.m.contents = new int[totalSize];
}

IntTensor::IntTensor(NumSizes numSizesV, ...)
    : IntTensor() {

    va_list sizesList;
    va_start(sizesList, numSizesV);
    fillSelf(numSizesV, sizesList);
    va_end(sizesList);
}

IntTensor::IntTensor(NumSizes numSizesV, va_list sizesList)
    : IntTensor() {

    fillSelf(numSizesV, sizesList);
}

void IntTensor::print(std::ostream &stream) const {
    stream << "IntTensor";
    stream << std::make_pair(
        payload_.m.sizes, payload_.m.sizes + payload_.m.numSizes);
}

void IntTensor::requestVisit(DataVisitor &visitor) const {
    visitor.visitIntTensor(*this);
}

inline void FloatTensor::fillSelf(
        NumSizes numSizesV, va_list sizesList) {

    payload_.m.numSizes = numSizesV.value;
    payload_.m.sizes = new int[payload_.m.numSizes];
    int totalSize = fillSizes(
        payload_.m.numSizes, sizesList, payload_.m.sizes);
    payload_.m.contents = new float[totalSize];
}

FloatTensor::FloatTensor(NumSizes numSizesV, ...)
    : FloatTensor() {

    va_list sizesList;
    va_start(sizesList, numSizesV);
    fillSelf(numSizesV, sizesList);
    va_end(sizesList);
}

FloatTensor::FloatTensor(NumSizes numSizesV, va_list sizesList)
    : FloatTensor() {

    fillSelf(numSizesV, sizesList);
}

void FloatTensor::print(std::ostream &stream) const {
    stream << "FloatTensor";
    stream << std::make_pair(
        payload_.m.sizes, payload_.m.sizes + payload_.m.numSizes);
}

void FloatTensor::requestVisit(DataVisitor &visitor) const {
    visitor.visitFloatTensor(*this);
}

void Branch::print(std::ostream &stream) const {
    stream << values_;
}

void Branch::requestVisit(DataVisitor &visitor) const {
    visitor.visitBranch(*this);
}

static inline IntTensor * asIntTensor(DataPtr &data) {
    return static_cast<IntTensor *>(data.get());
}

static inline FloatTensor * asFloatTensor(DataPtr &data) {
    return static_cast<FloatTensor *>(data.get());
}

static inline Branch * asBranch(DataPtr &data) {
    return static_cast<Branch *>(data.get());
}

static inline Branch * accessBranchR(BranchR *branch) {
    return static_cast<Branch *>(branch);
}

static inline Branch * accessBranchRW(BranchRW *branch) {
    return static_cast<Branch *>(branch);
}

int branchR_getSize(BranchR *branch) {
    return static_cast<Branch *>(branch)->size();
}

int branchRW_getSize(BranchRW *branch) {
    return static_cast<Branch *>(branch)->size();
}

IntTensorR * branchR_getIntTensor(BranchR *branch, int index) {
    return &asIntTensor(accessBranchR(branch)->values_[index])->getR();
}

IntTensorRW * branchRW_getIntTensor(BranchRW *branch, int index) {
    return &asIntTensor(accessBranchRW(branch)->values_[index])->getRW();
}

FloatTensorR * branchR_getFloatTensor(BranchR *branch, int index) {
    return &asFloatTensor(accessBranchR(branch)->values_[index])->getR();
}

FloatTensorRW * branchRW_getFloatTensor(BranchRW *branch, int index) {
    return &asFloatTensor(accessBranchRW(branch)->values_[index])->getRW();
}

BranchR * branchR_getBranch(BranchR *branch, int index) {
    return asBranch(accessBranchR(branch)->values_[index]);
}

BranchRW * branchRW_getBranch(BranchRW *branch, int index) {
    return asBranch(accessBranchRW(branch)->values_[index]);
}

IntTensorRW * branchRW_makeIntTensor(
        BranchRW *branch, int indexInclAppend, NumSizes numSizesV, ...) {

    Branch *branchObj = accessBranchRW(branch);
    IntTensorRW *result;

    va_list sizesList;
    va_start(sizesList, numSizesV);
    DataPtr data = make_unique<IntTensor>(numSizesV, sizesList);
    if (indexInclAppend == APPEND_INDEX) {
        branchObj->values_.push_back(std::move(data));
        result = &asIntTensor(branchObj->values_.back())->getRW();
    } else {
        DataPtr &value = branchObj->values_[indexInclAppend];
        value = std::move(data);
        result = &asIntTensor(value)->getRW();
    }
    va_end(sizesList);
    return result;
}

FloatTensorRW * branchRW_makeFloatTensor(
        BranchRW *branch, int indexInclAppend, NumSizes numSizesV, ...) {

    Branch *branchObj = accessBranchRW(branch);
    FloatTensorRW *result;

    va_list sizesList;
    va_start(sizesList, numSizesV);
    DataPtr data = make_unique<IntTensor>(numSizesV, sizesList);
    if (indexInclAppend == APPEND_INDEX) {
        branchObj->values_.push_back(std::move(data));
        result = &asFloatTensor(branchObj->values_.back())->getRW();
    } else {
        DataPtr &value = branchObj->values_[indexInclAppend];
        value = std::move(data);
        result = &asFloatTensor(value)->getRW();
    }
    va_end(sizesList);
    return result;
}

BranchRW * branchRW_makeBranch(
        BranchRW *branch, int indexInclAppend, int size) {

    Branch *branchObj = accessBranchRW(branch);
    BranchRW *result;

    DataPtr data = make_unique<Branch>(size);
    if (indexInclAppend == APPEND_INDEX) {
        branchObj->values_.push_back(std::move(data));
        result = asBranch(branchObj->values_.back());
    } else {
        DataPtr &value = branchObj->values_[indexInclAppend];
        value = std::move(data);
        result = asBranch(value);
    }
    return result;
}

void branchRW_pop(BranchRW *branch) {
    accessBranchRW(branch)->values_.pop_back();
}