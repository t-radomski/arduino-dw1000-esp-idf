/*
 * MIT License
 * 
 * Copyright (c) 2018 Michele Biondi, Andrea Salvatori
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * @file SPIporting.hpp
 * Arduino porting for the SPI interface.
*/

#include "SPIporting.hpp"
#include "DW1000NgConstants.hpp"
#include "DW1000NgRegisters.hpp"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "config.hpp"
#include <cstring>
#include "esp_log.h"
	
static spi_device_handle_t spi_handle;

namespace SPIporting {
	
	namespace {

        constexpr uint32_t EspSPImaximumSpeed = 20000000; //20MHz
        constexpr uint32_t ArduinoSPImaximumSpeed = 16000000; //16MHz
        constexpr uint32_t SPIminimumSpeed = 2000000; //2MHz

        // These SPISettings and _currentSPI are now unused with ESP-IDF, so can be removed

        // void _openSPI(uint8_t slaveSelectPIN) {
		// 	gpio_set_level(static_cast<gpio_num_t>(slaveSelectPIN), 0); // Assert CS (active low)
        // }

        // void _closeSPI(uint8_t slaveSelectPIN) {
		// 	gpio_set_level(static_cast<gpio_num_t>(slaveSelectPIN), 1); // Deassert CS
        // }
    }

	void SPIinit()
	{
		ESP_LOGI("SPIporting", "SPIinit() start");

		spi_bus_config_t buscfg = {};
		buscfg.mosi_io_num = Config::PIN_MOSI;
		buscfg.miso_io_num = Config::PIN_MISO;
		buscfg.sclk_io_num = Config::PIN_SCK;
		buscfg.quadwp_io_num = -1;
		buscfg.quadhd_io_num = -1;
		buscfg.max_transfer_sz = 0;

		spi_device_interface_config_t devcfg = {};
		devcfg.clock_speed_hz = 2 * 1000 * 1000; // 2 MHz to start
		devcfg.mode = 0;
		devcfg.spics_io_num = Config::PIN_SS;
		devcfg.queue_size = 1;
		devcfg.cs_ena_posttrans = 0;

		ESP_LOGI("SPIporting", "Initializing SPI bus on host SPI2_HOST...");
		esp_err_t ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
		if (ret == ESP_ERR_INVALID_STATE) {
			ESP_LOGW("SPIporting", "SPI bus already initialized, skipping...");
		} else if (ret != ESP_OK) {
			ESP_LOGE("SPIporting", "spi_bus_initialize failed: %s", esp_err_to_name(ret));
		} else {
			ESP_LOGI("SPIporting", "SPI bus initialized successfully.");
		}

		ESP_LOGI("SPIporting", "Adding DW1000 device on SS=%d...", (int)Config::PIN_SS);
		ret = spi_bus_add_device(SPI2_HOST, &devcfg, &spi_handle);
		if (ret != ESP_OK) {
			ESP_LOGE("SPIporting", "spi_bus_add_device failed: %s", esp_err_to_name(ret));
		} else {
			ESP_LOGI("SPIporting", "DW1000 device added successfully.");
		}

		ESP_LOGI("SPIporting", "SPIinit() done");
	}

	void SPIend() {
		spi_bus_remove_device(spi_handle);
		spi_bus_free(SPI2_HOST);
	}

	void SPIselect(uint8_t slaveSelectPIN) {
		gpio_set_level(static_cast<gpio_num_t>(slaveSelectPIN), 1); // Deselect
		gpio_set_direction(static_cast<gpio_num_t>(slaveSelectPIN), GPIO_MODE_OUTPUT);
	}
	void writeToSPI(uint8_t slaveSelectPIN, uint8_t headerLen, uint8_t *header, uint16_t dataLen, uint8_t *data) {
		uint8_t tx_buf[headerLen + dataLen];
		memcpy(tx_buf, header, headerLen);
		memcpy(tx_buf + headerLen, data, dataLen);

		spi_transaction_t t = {};
		t.length = static_cast<size_t>(8 * (headerLen + dataLen));
		t.tx_buffer = tx_buf;
		t.rx_buffer = nullptr;
		
		esp_err_t ret = spi_device_transmit(spi_handle, &t);
		if (ret != ESP_OK) {
			ESP_LOGE("SPIporting", "SPI transmit failed: %s", esp_err_to_name(ret));
		}
	}
    	void readFromSPI(uint8_t slaveSelectPIN, uint8_t headerLen, uint8_t *header, uint16_t dataLen, uint8_t *data) {
		uint8_t rx_buf[headerLen + dataLen];
		spi_transaction_t t = {};
		t.length = static_cast<size_t>(8 * (headerLen + dataLen));
		t.tx_buffer = header;
		t.rx_buffer = rx_buf;
		
		esp_err_t ret = spi_device_transmit(spi_handle, &t);
		if (ret != ESP_OK) {
			ESP_LOGE("SPIporting", "SPI receive failed: %s", esp_err_to_name(ret));
			return;
		}

		// Copy only the data portion to the output buffer
		memcpy(data, rx_buf + headerLen, dataLen);
	}

	void setSPIspeed(SPIClock speed) {
		spi_bus_remove_device(spi_handle);

		spi_device_interface_config_t devcfg = {};
		devcfg.command_bits = 0;
		devcfg.address_bits = 0;
		devcfg.dummy_bits = 0;
		devcfg.mode = 0;
		devcfg.clock_speed_hz = (speed == SPIClock::FAST) ? 20000000 : 2000000; // 20MHz or 2MHz
		devcfg.spics_io_num = Config::PIN_SS;
		devcfg.flags = 0;
		devcfg.queue_size = 1;
		devcfg.pre_cb = nullptr;
		devcfg.post_cb = nullptr;
		devcfg.cs_ena_pretrans = 0;
		devcfg.cs_ena_posttrans = 0;

		spi_bus_add_device(SPI2_HOST, &devcfg, &spi_handle);
	}

}