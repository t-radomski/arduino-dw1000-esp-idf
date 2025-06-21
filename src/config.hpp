#pragma once

#include <stdint.h>

namespace Config {
    // DW1000 configuration
    const uint8_t PIN_IRQ = 10; // irq pin
    const uint8_t PIN_RST = 11; // reset pin

    const uint8_t PIN_SS = 18;   // spi select pin (CS)
    const uint8_t PIN_MOSI = 19; // master out slave in (MOSI)
    const uint8_t PIN_MISO = 20; // master in slave out (MISO)
    const uint8_t PIN_SCK = 21; // clock pin (SCK)
}