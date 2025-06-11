#pragma once
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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

inline void platform_delay(uint32_t ms) {
    vTaskDelay(ms / portTICK_PERIOD_MS);
}