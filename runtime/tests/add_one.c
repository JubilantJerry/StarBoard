#include "native_interface.h"

void addOne(DataBlock *block) {
    IntTensorStruct const *data = moveToOutputIntTensor(block, 0, 0);
    data->contents[0]++;
}