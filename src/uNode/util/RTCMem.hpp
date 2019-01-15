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
#ifndef RTCMEM_UTIL
#define RTCMEM_UTIL
#include <stdint.h>

/**
 * Maximum number of bytes that can be written to RTC memory in reliable way
 *
 * We are using 4 bytes to reliably store the value of a single byte. This means
 * that the 512 bytes available for user data can represent 128 bytes of info
 */
#define RTCMEM_MAX_SLOT 127

/**
 * Known slots in the project
 */
#define RTCMEM_SLOT_REBOOTS     RTCMEM_MAX_SLOT       // 1-slot wide
#define RTCMEM_SLOT_BOOTFLAGS   RTCMEM_MAX_SLOT - 1   // 1-slot wide
#define RTCMEM_SLOT_LORAPERSIST RTCMEM_MAX_SLOT - 13  // 12-slot wide
#define RTCMEM_SLOT_SKETCHID    RTCMEM_MAX_SLOT - 14  // 1-slot wide

/**
 * A flag that denotes that the system went to sleep because of undervoltage
 * protection
 */
#define BOOTFLAG_UNDERVOLTAGE_PROTECTION  1

/**
 * A flag that denotes that the LoRa module has
 */
#define BOOTFLAG_LORA_JOINED              2

/**
 * Check the RTC memory status and if it's in an invalid state, restart
 */
void rtcmem_setup();

/**
 * Read a verified value from the RTC memory (24 usable bits)
 */
uint32_t rtcMemVeriRead(const uint8_t slot, const uint32_t defaultValue = 0);

/**
 * Write a verified value to the RTC memory (24 usable bits)
 */
void rtcMemVeriWrite(const uint8_t slot, const uint32_t value);

/**
 * Set a flag to a verified RTC slot
 */
void rtcMemFlagSet(const uint8_t slot, const uint32_t flag);

/**
 * Unset a flag to a verified RTC slot
 */
void rtcMemFlagUnset(const uint8_t slot, const uint32_t flag);

/**
 * Read a flag to a verified RTC slot
 */
uint32_t rtcMemFlagGet(const uint8_t slot, const uint32_t flag);

/**
 * Invalidates an RTC slot
 */
void rtcMemInvalidate(const uint8_t slot);

/**
 * Invalidates the entire RTC memory
 */
void rtcMemInvalidateAll();

/**
 * Write arbitrary structures or values on the RTC memory (up to 255 bytes)
 *
 * Returns the number of bytes written or 0 in case of error.
 */
uint8_t rtcMemWrite(const uint8_t slot, const uint8_t offset, uint32_t &value);
uint8_t rtcMemWrite(const uint8_t slot, const uint8_t offset, uint16_t &value);
uint8_t rtcMemWrite(const uint8_t slot, const uint8_t offset, uint8_t &value);
template <typename T> uint8_t rtcMemWrite(const uint8_t slot, const uint8_t offset, T &value) {
  uint32_t chunk;
  uint8_t *value_ptr = static_cast<uint8_t*>(static_cast<void*>(&value));
  uint8_t s = slot;

  for (uint16_t i = 0; i < sizeof(T); i += sizeof(uint32_t), s++) {
    if (i + sizeof(uint32_t) <= sizeof(T)) { // Whole chunk?
      memcpy(&chunk, value_ptr + i, sizeof(uint32_t));
      ESP.rtcUserMemoryWrite(s, &chunk, sizeof(uint32_t));

    } else { // Partial chunk
      ESP.rtcUserMemoryRead(s, &chunk, sizeof(chunk));
      memcpy(&chunk, value_ptr + i, sizeof(T) % sizeof(uint32_t));
      ESP.rtcUserMemoryWrite(s, &chunk, sizeof(uint32_t));
    }
  }

  return sizeof(T);
}

/**
 * Reads arbitrary structures or values on the RTC memory (up to 255 bytes)
 *
 * Returns the number of bytes read or 0 in case of error.
 */
uint8_t rtcMemRead(const uint8_t slot, const uint8_t offset, uint32_t &value);
uint8_t rtcMemRead(const uint8_t slot, const uint8_t offset, uint16_t &value);
uint8_t rtcMemRead(const uint8_t slot, const uint8_t offset, uint8_t &value);
template <typename T> uint8_t rtcMemRead(const uint8_t slot, const uint8_t offset, T &value) {
  uint32_t chunk;
  uint8_t *value_ptr = static_cast<uint8_t*>(static_cast<void*>(&value));
  uint8_t s = slot;

  for (uint16_t i = 0; i < sizeof(T); i += sizeof(uint32_t), s++) {
    if (i + sizeof(uint32_t) <= sizeof(T)) { // Whole chunk?
      ESP.rtcUserMemoryRead(s, &chunk, sizeof(chunk));
      memcpy(value_ptr + i, &chunk, sizeof(uint32_t));

    } else { // Partial chunk
      ESP.rtcUserMemoryRead(s, &chunk, sizeof(chunk));
      memcpy(value_ptr + i, &chunk, sizeof(T) % sizeof(uint32_t));
    }
  }

  return sizeof(T);
}


#endif
