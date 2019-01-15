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
#include "../uNodeOpen.hpp"
#include "PublicDefinitions.hpp"
#include "peripherals/GPIO.hpp"
#include "peripherals/LoRa.hpp"
#include "peripherals/Power.hpp"
#include "peripherals/Wire.hpp"
#include "util/Undervoltage.hpp"
#include "util/Health.hpp"
#include "util/RTCMem.hpp"

extern "C" {
  #include "user_interface.h"
}

#define DEBUG_CONTEXT "uNode"
#include "util/Debug.hpp"

/**
 * Singleton constructor
 */
__attribute__((weak)) uNodeClassOpen uNode;

/**
 * Initialize the micro-node hardware
 */
void uNodeClassOpen::setup() {

  // Initialize core structures first
  system_health_setup();
  system_config_setup();

  // Set configuration defaults if they are missing
  if (CONFIG_DEFAULT == system_config.lora.tx_sf) system_config.lora.tx_sf = LORA_SF7;
  if (CONFIG_DEFAULT == system_config.lora.tx_power) system_config.lora.tx_power = 14;
  if (CONFIG_DEFAULT == system_config.lora.tx_timeout) system_config.lora.tx_timeout = 10000;
  if (CONFIG_DEFAULT == system_config.lora.tx_retries) system_config.lora.tx_retries = 3;
  if (CONFIG_DEFAULT == system_config.logging.level)  system_config.logging.level = LOG_LEVEL_INFO;
  if (CONFIG_DEFAULT == system_config.logging.baud)  system_config.logging.baud = 115200;
  if (CONFIG_DEFAULT == system_config.undervoltageProtection.disableThreshold) system_config.undervoltageProtection.disableThreshold = 3100;
  if (CONFIG_DEFAULT == system_config.undervoltageProtection.enableThreshold) system_config.undervoltageProtection.enableThreshold = 3200;

  // Initialize power management before trying to check battery status. This
  // also makes sure that we don't wake-up with RF on.
  Power.begin();

  // Check for undervoltage and if such condition is met, shut the system down
  if (system_config.undervoltageProtection.disableThreshold != 0xFFFF) {
    delay(100); // Wait for voltage to stabilize
    undervoltageCheckLockdown(); // Maintain lock-down if it's active
    undervoltageProtect(); // Check if we should enter a lock-down
  }

  // Bare minimum hardware initialization
  Serial.begin(system_config.logging.baud);

  // Make sure we are running on low speed
  system_update_cpu_freq(80);
  logDebug("");  // Start at new line after ESP boot garbage
  logDebug("Booted firmware v" UNODE_FIRMWARE_VERSION);
  if (system_config.undervoltageProtection.disableThreshold != 0xFFFF) {
    logDebug("VCC measured at %d mV", ESP.getVcc());
  }

  // Initialize the RTC memory
  rtcmem_setup();
}

/**
 * Update micro-node interfaces
 */
void uNodeClassOpen::step() {
  LoRa.step();
  if (system_config.undervoltageProtection.disableThreshold != 0xFFFF) {
    undervoltageProtect();
  }
}

/**
 * Put all peripherals on idle
 */
void uNodeClassOpen::standby(STANDBY_MODE_t mode) {
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
    if (mode & STANDBY_VBUS) {
      Power.setVBusOverride(0);
    }
  }
}

/**
 * Send a packet over the LoRa network
 */
void uNodeClassOpen::sendLoRa(const char * data, size_t size, fnLoRaDataCallback whenDone) {
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
void uNodeClassOpen::deepSleep(const uint16_t seconds) {
  Power.off();
  logDebug("Sleeping for %d sec", seconds);
  Serial.flush();
  ESP.deepSleep(seconds * 1e6, WAKE_RF_DEFAULT);
}

/**
 * Set the pin direction on the GPIO chip
 */
void uNodeClassOpen::pinMode(uint8_t pin, uint8_t mode) {
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
void uNodeClassOpen::digitalWrite(uint8_t pin, uint8_t value) {
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
int uNodeClassOpen::digitalRead(uint8_t pin) {
  if (pin < 100) {  // Physical pin
    return ::digitalRead(pin);
  } else { // Expansion pin
    Power.setGPIO(1);
    return GPIO.digitalRead(pin - 100);
  }
}

  /**
   * Time a half-pulse on a pin (max 500 microseconds)
   */
int uNodeClassOpen::timeHalfPulse(uint8_t pin) {
  if (pin < 100) {  // Physical pin
    return -1;
  } else { // Expansion pin
    Power.setGPIO(1);
    return GPIO.timeHalfPulse(pin - 100);
  }
}

/**
 * Enable or Disable the VBus explicitly
 */
void uNodeClassOpen::enablePeripherals(const uint8_t enabled) {
  Power.setVBusOverride(enabled);
}

/**
 * Blink the built-in led
 */
void uNodeClassOpen::blink(const uint16_t on_ms, const uint16_t off_ms, const uint8_t cycles) {
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
