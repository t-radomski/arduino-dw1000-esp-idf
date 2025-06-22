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

typedef struct ieee154_tag_broadcast_message {
    uint8_t frameCtrl[2];      //  0, 2: Frame control bytes 00-01: 0x01 (Frame Type 0x01=data), 0xC8 (0xC0=src extended address 64 bits, 0x08=dest address 16 bits)
    uint8_t seq8;              //  2, 1: Sequence number 02
    uint8_t panID[2];          //  3, 2: PAN ID 03-04
    uint8_t destAddr[2];       //  5, 2: 0xFFFF
    uint8_t sourceAddr[8];     //  7, 8: 64-bit EUI address
    uint8_t messageType;       // 15, 1: Message Type = RTLS_MSG_TYPE_TAG_MIN_RANGE_V2
    uint8_t seq32_3[3];        // 16, 3: High 3 bytes of the tag's ranging message sequence number, combined with seq8 to form seq32, which allows a larger max value than seq8 alone
    uint8_t switchStatus;      // 19, 1: Switch status
    uint8_t fcs[2];            // 20, 2: Space for CRC, logically part of the message. ScenSor TX calculates and adds these bytes.
} TAG_BROADCAST_MESSAGE;      // Total: 22 bytes

enum message_type {
    TDOA_BROADCAST = 0x01 // Broadcast message type
};