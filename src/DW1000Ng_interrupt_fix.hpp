/*
 * DW1000Ng Interrupt Handling Fix for ESP-IDF
 * Header file
 */

#pragma once

#include "esp_err.h"

/**
 * Initialize the fixed interrupt system for DW1000Ng
 * This replaces the problematic direct ISR approach
 */
esp_err_t init_fixed_interrupt_system();

/**
 * Setup GPIO interrupt with proper ESP-IDF task-based handling
 * @param irq_pin GPIO pin number for DW1000 IRQ
 */
esp_err_t setup_fixed_gpio_interrupt(int irq_pin);