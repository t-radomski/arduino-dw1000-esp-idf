/*
 * DW1000Ng Interrupt Handling Fix for ESP-IDF
 * 
 * This file contains the corrected interrupt handling that avoids
 * doing complex operations in ISR context.
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "DW1000Ng.hpp"

// Global variables for the fixed interrupt system
static QueueHandle_t interrupt_queue = NULL;
static TaskHandle_t interrupt_task_handle = NULL;
static const char* TAG_IRQ = "DW1000_IRQ";

// Minimal ISR - just signals that an interrupt occurred
static void IRAM_ATTR gpio_isr_handler(void* arg) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint32_t interrupt_signal = 1;
    
    // Just send a signal to the interrupt handling task
    xQueueSendFromISR(interrupt_queue, &interrupt_signal, &xHigherPriorityTaskWoken);
    
    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
}

// Task that handles the actual interrupt processing
static void interrupt_handling_task(void* pvParameters) {
    uint32_t interrupt_signal;
    
    while (1) {
        // Wait for interrupt signal from ISR
        if (xQueueReceive(interrupt_queue, &interrupt_signal, portMAX_DELAY) == pdTRUE) {
            // Now we're in task context, safe to do complex operations
            ESP_LOGD(TAG_IRQ, "Processing DW1000 interrupt in task context");
            
            // Call the original interrupt service routine logic
            // but now safely in task context
            DW1000Ng::interruptServiceRoutine();
        }
    }
}

// Initialize the fixed interrupt system
esp_err_t init_fixed_interrupt_system() {
    // Create queue for interrupt signals
    interrupt_queue = xQueueCreate(10, sizeof(uint32_t));
    if (interrupt_queue == NULL) {
        ESP_LOGE(TAG_IRQ, "Failed to create interrupt queue");
        return ESP_FAIL;
    }
    
    // Create task to handle interrupts
    BaseType_t ret = xTaskCreate(
        interrupt_handling_task,
        "dw1000_irq",
        4096,  // Stack size
        NULL,
        configMAX_PRIORITIES - 1,  // High priority
        &interrupt_task_handle
    );
    
    if (ret != pdPASS) {
        ESP_LOGE(TAG_IRQ, "Failed to create interrupt handling task");
        return ESP_FAIL;
    }
    
    ESP_LOGI(TAG_IRQ, "Fixed interrupt system initialized");
    return ESP_OK;
}

// Replace the problematic GPIO ISR registration
esp_err_t setup_fixed_gpio_interrupt(int irq_pin) {
    // Initialize the fixed interrupt system first
    esp_err_t ret = init_fixed_interrupt_system();
    if (ret != ESP_OK) {
        return ret;
    }
    
    // Configure GPIO
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << irq_pin);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
    
    // Install ISR service if not already installed
    gpio_install_isr_service(0);
    
    // Add our minimal ISR handler
    gpio_isr_handler_add((gpio_num_t)irq_pin, gpio_isr_handler, NULL);
    
    ESP_LOGI(TAG_IRQ, "Fixed GPIO interrupt configured on pin %d", irq_pin);
    return ESP_OK;
}
