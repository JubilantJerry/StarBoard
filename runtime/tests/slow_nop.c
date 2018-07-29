#include <time.h>
#include "data_interface.h"

void slowNop(DataBlock *block) {
    outputMsg_moveIntTensor(block, 0);
    
    struct timespec time = {0, 1000000L};
    nanosleep(&time, NULL);
}