#include "native_interface.h"

void addOne(DataBlock *block) {
    IntTensorRW *data = outputMsg_moveIntTensor(block, 0);
    data->contents[0]++;
}