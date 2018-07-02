#include "native_interface.h"

void addOne(DataBlock *block) {
    IntTensorRW *data = output_moveIntTensor(block, 0, 0);
    data->contents[0]++;
}