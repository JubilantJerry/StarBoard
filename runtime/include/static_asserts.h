#ifndef STATIC_ASSERTS_H
#define STATIC_ASSERTS_H

static void assertDataSizes() {
    (void)assertDataSizes;
    switch(0) {
        case 0: break;
        case (sizeof(int) == 4? 1 : 0): return;
        case (sizeof(float) == 4? 2 : 0): return;
    }
}

#if __BYTE_ORDER != __LITTLE_ENDIAN
#error "System endianness not supported!"
#endif

#endif