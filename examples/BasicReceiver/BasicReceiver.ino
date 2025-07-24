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
*/

/*
 * Copyright (c) 2015 by Thomas Trojer <thomas@trojer.net>
 * Decawave DW1000 library for arduino.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file BasicReceiver.ino
 * Use this to test simple sender/receiver functionality with two
 * DW1000:: Complements the "BasicSender" example sketch.
 * 
 * @todo
 *  - move strings to flash (less RAM consumption)
 *  
 */

#include <DW1000Ng.hpp>

const uint8_t PIN_RST = 17; // reset pin
const uint8_t PIN_IRQ = 16; // irq pin
const uint8_t PIN_MISO = 10;
const uint8_t PIN_MOSI = 11;
const uint8_t PIN_SCK = 12;
const uint8_t PIN_CS = 13;

SPIClass *_spi = nullptr;

int16_t numReceived = 0; // todo check int type
String message;

device_configuration_t DEFAULT_CONFIG = {
    false,
    true,
    true,
    true,
    false,
    SFDMode::STANDARD_SFD,
    Channel::CHANNEL_5,
    DataRate::RATE_850KBPS,
    PulseFrequency::FREQ_16MHZ,
    PreambleLength::LEN_256,
    PreambleCode::CODE_3
};

void setup() {
  // DEBUG monitoring
  Serial.begin(9600);
  ESP_LOGI(TAG_TWR, "%s",F("### DW1000Ng-arduino-receiver-test ###"));
  _spi = new SPIClass();
  _spi->begin(PIN_SCK, PIN_MISO, PIN_MOSI, PIN_CS);
  // initialize the driver
  DW1000Ng::initialize(PIN_CS, PIN_IRQ, PIN_RST, *_spi);
  ESP_LOGI(TAG_TWR, "%s",F("DW1000Ng initialized ..."));

  DW1000Ng::applyConfiguration(DEFAULT_CONFIG);

  DW1000Ng::setDeviceAddress(6);
  DW1000Ng::setNetworkId(10);

  DW1000Ng::setAntennaDelay(16436);
  ESP_LOGI(TAG_TWR, "%s",F("Committed configuration ..."));
  // DEBUG chip info and registers pretty printed
  char msg[128];
  DW1000Ng::getPrintableDeviceIdentifier(msg);
  ESP_LOGI(TAG_TWR, "%s","Device ID: "); ESP_LOGI(TAG_TWR, "%s",msg);
  DW1000Ng::getPrintableExtendedUniqueIdentifier(msg);
  ESP_LOGI(TAG_TWR, "%s","Unique ID: "); ESP_LOGI(TAG_TWR, "%s",msg);
  DW1000Ng::getPrintableNetworkIdAndShortAddress(msg);
  ESP_LOGI(TAG_TWR, "%s","Network ID & Device Address: "); ESP_LOGI(TAG_TWR, "%s",msg);
  DW1000Ng::getPrintableDeviceMode(msg);
  ESP_LOGI(TAG_TWR, "%s","Device mode: "); ESP_LOGI(TAG_TWR, "%s",msg);
}

void loop() {
  DW1000Ng::startReceive();
  while(!DW1000Ng::isReceiveDone()) {
    #if defined(ESP8266)
    yield();
    #endif
  }
  DW1000Ng::clearReceiveStatus();
  numReceived++;
  // get data as string
  DW1000Ng::getReceivedData(message);
  ESP_LOGI(TAG_TWR, "%s","Received message ... #"); ESP_LOGI(TAG_TWR, "%s",numReceived);
  ESP_LOGI(TAG_TWR, "%s","Data is ... "); ESP_LOGI(TAG_TWR, "%s",message);
  ESP_LOGI(TAG_TWR, "%s","RX power is [dBm] ... "); ESP_LOGI(TAG_TWR, "%s",DW1000Ng::getReceivePower());
  ESP_LOGI(TAG_TWR, "%s","Signal quality is ... "); ESP_LOGI(TAG_TWR, "%s",DW1000Ng::getReceiveQuality());
}
