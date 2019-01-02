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
#define UNODE_FIRMWARE_MINOR    6
#define UNODE_FIRMWARE_VERSION  "0.6"

/**
 * The callback function to use for letting the user know when a transmission is done
 */
typedef void(*fnLoRaCallback)(int status);

/**
 * Standby mode enum constant
 */
typedef enum {
  STANDBY_ALL    = 7,   // Put all peripherals on standby
  STANDBY_GPIO   = 1,   // Put GPIO expansion on standby
  STANDBY_WIFI   = 2,   // Put WiFi on standby
  STANDBY_LORA   = 4    // Put LoRa on standby
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
 * Debug log level
 */
typedef enum {
  LOG_DEFAULT   = 0,    // Default logging value
  LOG_DISABLED  = 1,    // Do not echo anything on the serial port
  LOG_INFO      = 2,    // Info-level messages on the serial port
} LOG_LEVEL_t;

#endif
