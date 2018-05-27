/** @file test_lib.hpp
 *  @brief A test header file
 */

#ifndef NATIVE_INTERFACE_H
#define NATIVE_INTERFACE_H

#define I(multiplier, index, remain) (multiplier * index + remain)

typedef struct {
    int numSizes;
    int *sizes;
    int *contents;
} IntTensorStruct;

typedef union {
    IntTensorStruct *intTensor;
} DataUnion;

#endif