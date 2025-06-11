#pragma once

#include <stdint.h>

namespace Config {
    // DW1000 configuration
    const uint8_t PIN_RST = 4; // reset pin
    const uint8_t PIN_IRQ = 16; // irq pin
    
    const uint8_t PIN_SS = 15;   // spi select pin (CS)
    const uint8_t PIN_SCK = 18; // clock pin (SCK)
    const uint8_t PIN_MISO = 2; // master in slave out (MISO)
    const uint8_t PIN_MOSI = 23; // master out slave in (MOSI)
}