#include "native_interface.h"

void addOne(DataBlock *block) {
    IntTensorStruct const *value = moveToOutputIntTensor(block, 0, 0);
    value->contents[0]++;
}