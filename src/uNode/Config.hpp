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
#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include "PublicDefinitions.hpp"

/**
 * A general-purpose constant that the user can use for setting a default config
 */
#define  CONFIG_DEFAULT    0

/**
 * The LoRA Configuration parameters
 */
struct uNodeConfigLora {

  /**
   * LoRA mode
   */
  LORA_MODE_t         mode;

  /**
   * Configuration for various modes
   */
  union {

    /**
     * ABP Activation Details
     */
    struct {
      uint8_t             appKey[16];
      uint8_t             netKey[16];
      uint32_t            devAddr;
    } abp;

    /**
     * OTAA Activation Details
     */
    struct {
      uint8_t             appKey[16];
      uint8_t             appEui[8];
      uint8_t             devEui[9];
    } otaa;

  } activation;

  /**
   * The spreading factor to use (default is LORA_SF7)
   */
  LORA_SPREADFACTOR_t tx_sf;

  /**
   * The transmission power to use (default is 14)
   */
  uint8_t             tx_power;

  /**
   * When using managed transmission, this defines the number of seconds to wait
   * for a transmission acknowledgment before considering it timed out.
   */
  uint16_t            tx_timeout;

  /**
   * When using managed transmission, this defines the number of re-transmission
   * attempts to perform, before bailing out.
   */
  uint8_t             tx_retries;

  /**
   * Enable or disable Adaptive Data Rate (ADR) on the LoRa Chip
   */
  uint8_t             adr;

};

/**
 * The LoRA Configuration parameters
 */
struct uNodeConfigUV {

  /**
   * Cut-off voltage (low)
   */
  uint16_t            disableThreshold;

  /**
   * Activation-off voltage (high)
   */
  uint16_t            enableThreshold;

};

/**
 * Debug log structure
 */
struct uNodeConfigLogging {

  /**
   * The logging level
   */
  LOG_LEVEL_t  level;

  /**
   * Baud rate for the serial console
   */
  uint32_t      baud;

};

/**
 * The device configuration
 */
struct uNodeConfig {

  /**
   * The LoRA Configuration
   */
  uNodeConfigLora       lora;

  /**
   * Serial port debug level
   */
  uNodeConfigLogging    logging;

  /**
   * Under-voltage protection (for LiPo Coin-Cell Batteries)
   */
  uNodeConfigUV         undervoltageProtection;

};

/**
 * The configuration that should be given by the user
 */
extern uNodeConfig unode_config;

#endif
