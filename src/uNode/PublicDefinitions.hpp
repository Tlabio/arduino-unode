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
#ifndef PUBLIC_DEFINITIONS_H
#define PUBLIC_DEFINITIONS_H
/**
 * This file contains the definitions that are exposed to the user
 */

/**
 * Current firmware version
 */
#define UNODE_FIRMWARE_MAJOR    0
#define UNODE_FIRMWARE_MINOR    8
#define UNODE_FIRMWARE_VERSION  "0.8"

/**
 * The callback function to use for letting the user know when a transmission is done
 */
typedef void(*fnLoRaCallback)(int status);
typedef void(*fnLoRaDataCallback)(int status, uint8_t *data, uint8_t len);

/**
 * Standby mode enum constant
 */
typedef enum {
  STANDBY_ALL    = 7,   // Put all peripherals on standby
  STANDBY_GPIO   = 1,   // Put GPIO expansion on standby
  STANDBY_WIFI   = 2,   // Put WiFi on standby
  STANDBY_LORA   = 4,   // Put LoRa on standby
  STANDBY_VBUS   = 8    // Put VBus on standby, if not used by LoRa or GPIO
} STANDBY_MODE_t;

/**
 * Lora mode enum constant
 */
typedef enum {
  LORA_DISABLED  = 0,   // Disable LoRa capabilities entirely
  LORA_TTN_ABP   = 1,   // Use The-Things-Network ABP Activation
  LORA_TTN_OTAA  = 2    // Use The-Things-Network OTAA Activation
} LORA_MODE_t;

/**
 * Log level constants
 */
typedef enum {
  LOG_LEVEL_DEFAULT   = 0,    // Default logging value
  LOG_LEVEL_DISABLED  = 1,    // Do not echo anything on the serial port
  LOG_LEVEL_INFO      = 2,    // Info-level messages on the serial port
} LOG_LEVEL_t;

/**
 * Log structure constants, syntax-compatible with v0.7.0
 */
#define LOG_DEFAULT     { LOG_LEVEL_DEFAULT, 115200 }
#define LOG_DISABLED    { LOG_LEVEL_DISABLED, 115200 }
#define LOG_INFO        { LOG_LEVEL_INFO, 115200 }
#define LOG_INFO_74880  { LOG_LEVEL_INFO, 74880 }
#define LOG_INFO_9600   { LOG_LEVEL_INFO, 9600 }

/**
 * Spreading factor for uNode
 */
typedef enum {
  LORA_SF_DEFAULT=0,
  LORA_SF12,
  LORA_SF11,
  LORA_SF10,
  LORA_SF9,
  LORA_SF8,
  LORA_SF7,
  LORA_SF7B
} LORA_SPREADFACTOR_t;

/**
 * Constants for the uNodeUVConfig
 */
#define UNDERVOLTAGE_DEFAULT  { CONFIG_DEFAULT, CONFIG_DEFAULT }
#define UNDERVOLTAGE_DISABLED { 0xFFFF, 0xFFFF }

/**
 * A structure that carries the system health check within 2 bytes
 */
struct system_health_t {

  /**
   * System VCC voltage (in millivolts) between 0~4095
   */
  uint16_t      vcc : 12;

  /**
   * A boolean flag that when set to 1, it means that the user-provided
   * health check function has detected an error.
   */
  uint8_t       error : 1;

  /**
   * If the user-provided health check returned no error, this variable
   * contains the reset reason. Otherwise it's defined by the user function.
   */
  uint8_t       reason : 3;

};

/**
 * User-overridable system health-check function
 *
 * If this function returns `0`, then the system is assumed to be healthy.
 * Otherwise a 3-bit error code should be returned (1 to 7), indicating the
 * reason of the error.
 */
uint8_t __attribute__((weak)) system_health_check();

#endif
