/*******************************************************************************
 * Copyright (c) 2018 Ioannis Charalampidis
 *
 * This is a private, preview release of the uNode hardware abstraction library.
 * The holder of a copy of this software and associated documentation files
 * (the "Software") is allowed to use the Software without any obligation to
 * create private and/or commercial projects. The Software can be obtained
 * through the official channels of the author, including but not limited to
 * Github and the official TLab.gr website. It is FORBIDDEN however to modify,
 * reverse-engineer, publish, distribute, sublicense, and/or sell copies of the
 * Software itself.
 *
 * The license for this file might change in a future release. The author is not
 * obliged to announce this change through any channel but it should be included
 * in the release notes.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *******************************************************************************/
#include "../uNode.hpp"
#include "PublicDefinitions.hpp"
#include "peripherals/GPIO.hpp"
#include "peripherals/LoRa.hpp"
#include "peripherals/Power.hpp"
#include "util/Undervoltage.hpp"

extern "C" {
  #include "user_interface.h"
}

#define DEBUG_CONTEXT "uNode"
#include "util/Debug.hpp"

/**
 * Singleton constructor
 */
__attribute__((weak)) uNodeClass uNode;

/**
 * Initialize the micro-node hardware
 */
void uNodeClass::setup() {

  // Initialize dynamic system config
  system_config_init();

  // Set configuration defaults if they are missing
  if (system_config.lora.tx_timeout == 0) system_config.lora.tx_timeout = 10000;
  if (system_config.lora.tx_retries == 0) system_config.lora.tx_retries = 10;
  if (system_config.serialLogLevel == LOG_DEFAULT)  system_config.serialLogLevel = LOG_INFO;

  // Initialize power management before trying to check battery status. This
  // also makes sure that we don't wake-up with RF on.
  Power.begin();

  // Check for undervoltage and if such condition is met, shut the system down
  if (system_config.undervoltageProtection) {
    delay(100); // Wait for voltage to stabilize
    undervoltageProtect();
  }

  // Bare minimum hardware initialization
  Serial.begin(115200);

  // Make sure we are running on low speed
  system_update_cpu_freq(80);
  logDebug("Booted firmware v" UNODE_FIRMWARE_VERSION);
}

/**
 * Update micro-node interfaces
 */
void uNodeClass::step() {
  LoRa.step();
  if (system_config.undervoltageProtection) {
    undervoltageProtect();
  }
}

/**
 * Put all peripherals on idle
 */
void uNodeClass::standby(STANDBY_MODE_t mode) {
  if ((mode & 0x7) == STANDBY_ALL) {
    Power.off();
  } else {
    if (mode & STANDBY_GPIO) {
      Power.setGPIO(0);
    }
    if (mode & STANDBY_LORA) {
      Power.setLoRaRadio(0);
    }
    if (mode & STANDBY_WIFI) {
      Power.setWiFiRadio(0);
    }
  }
}

/**
 * Send a packet over the LoRa network
 */
void uNodeClass::sendLoRa(const char * data, size_t size, fnLoRaCallback whenDone) {
  if (system_config.lora.mode == LORA_DISABLED) {
    return;
  }

  // Power up LoRa if not done already
  Power.setLoRaRadio(1);

  // Define the callback function to trigger when the transmission is completed
  if (whenDone != nullptr) LoRa.whenSent(whenDone);

  // Managed transmission is going to retry 10 times, using 10 seconds interval
  // between transmissions
  LoRa.sendManaged(data, size, system_config.lora.tx_retries,
                   system_config.lora.tx_timeout);
}

/**
 * Enter deep sleep
 */
void uNodeClass::deepSleep(const uint16_t seconds) {
  Power.off();
  ESP.deepSleep(seconds * 1e6, WAKE_RF_DEFAULT);
}

/**
 * Set the pin direction on the GPIO chip
 */
void uNodeClass::pinMode(uint8_t pin, uint8_t mode) {
  if (pin < 100) {  // Physical pin
    ::pinMode(pin, mode);
  } else { // Expansion pin
    Power.setGPIO(1);
    GPIO.pinMode(pin - 100, mode);
  }
}

/**
 * Write a logical value on the pin
 */
void uNodeClass::digitalWrite(uint8_t pin, uint8_t value) {
  if (pin < 100) {  // Physical pin
    ::digitalWrite(pin, value);
  } else { // Expansion pin
    Power.setGPIO(1);
    GPIO.digitalWrite(pin - 100, value);
  }
}

/**
 * Read a logical value on the pin
 */
int uNodeClass::digitalRead(uint8_t pin) {
  if (pin < 100) {  // Physical pin
    return ::digitalRead(pin);
  } else { // Expansion pin
    Power.setGPIO(1);
    return GPIO.digitalRead(pin - 100);
  }
}

/**
 * Blink the built-in led
 */
void uNodeClass::blink(const uint16_t on_ms, const uint16_t off_ms, const uint8_t cycles) {
  ::pinMode(UPIN_GPIO, OUTPUT);
  for (uint8_t i = 0; i < cycles; ++i) {
    ::digitalWrite(UPIN_GPIO, LOW);
    delay(on_ms);
    ::digitalWrite(UPIN_GPIO, HIGH);
    if (off_ms == 0) {
      delay(on_ms);
    } else {
      delay(off_ms);
    }
  }
}
