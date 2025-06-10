#pragma once
#include <stdint.h>

typedef uint8_t byte;

// Add bitSet, bitClear, bitRead macros if not defined
#ifndef bitSet
#define bitSet(value, bit)   ((value) |= (1UL << (bit)))
#endif
#ifndef bitClear
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#endif
#ifndef bitRead
#define bitRead(value, bit)  (((value) >> (bit)) & 0x01)
#endif