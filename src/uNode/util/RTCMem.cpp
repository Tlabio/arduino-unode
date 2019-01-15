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
#include <Arduino.h>
#include "RTCMem.hpp"
#include "Checksums.hpp"

#define DEBUG_CONTEXT "RTCMem"
#include "../util/Debug.hpp"

/**
 * The initial CRC value, that must be non-zero in order to result into a
 * non-zero CRC4 checksum on empty data.
 */
const uint8_t CRC_INITIAL = 0xA;

/**
 * Checks if a triple-copy checksum is valid
 */
bool csum_is_valid(uint32_t & data) {
  return (data & 0xF) == crc4(CRC_INITIAL, data >> 4, 28);
}

/**
 * Returns the value from a triple-copy structure
 */
uint32_t csum_get(uint32_t & data) {
  return data >> 4;
}

/**
 * Sets the value to a triple-copy structure
 */
void csum_set(uint32_t & data, uint32_t value) {
  uint8_t v1 = crc4(CRC_INITIAL, value, 28);
  data = (value << 4) | v1;
}

/**
 * Increments the value of the triple-copy structure by one. If the value
 * is not valid, it assumes it to be 0.
 */
uint32_t csum_inc(uint32_t & data) {
  uint8_t value = csum_get(data) + 1;
  csum_set(data, value);
  return value;
}

/**
 * Check the state of the RTC memory and reset it to zero if it's invalid
 */
void rtcmem_setup() {
  uint8_t bytes[32];

  // Read the MD5 checksum of the sketch and calculate a CRC16 from it
  ESP.getSketchMD5().getBytes(bytes, 32);
  uint16_t sketch_crc16 = crc16(bytes, 32);
  uint32_t saved_crc16 = rtcMemVeriRead(RTCMEM_SLOT_SKETCHID);

  // If the sketch checksum is not the same with the checksum stored in the
  // RTC memory, it means that we were flashed with a new firmware. Reset the
  // memory state if this happens.
  if (saved_crc16 != sketch_crc16) {
    logDebug("Reseting RTC RAM due to invalid sketch checksum");
    rtcMemInvalidateAll();
  }

  // Save the new sketch memory
  rtcMemVeriWrite(RTCMEM_SLOT_SKETCHID, sketch_crc16);
}

/**
 * Read a verified value from the RTC memory
 */
uint32_t rtcMemVeriRead(const uint8_t slot, const uint32_t defaultValue) {
  uint32_t data;

  // Read a 32-bit integer from the designated offset
  ESP.rtcUserMemoryRead(slot, &data, sizeof(data));

  // Return default value if value is not valid
  if (!csum_is_valid(data)) {
    return defaultValue;
  }

  // Return the de-composed value
  return csum_get(data);
}

/**
 * Write a verified value to the RTC memory
 */
void rtcMemVeriWrite(const uint8_t slot, const uint32_t value) {
  uint32_t data;

  // Define the value including checksum guards
  csum_set(data, value);

  // Write memory
  ESP.rtcUserMemoryWrite(slot, &data, sizeof(data));
}

/**
 * Set a flag to a verified RTC slot
 */
void rtcMemFlagSet(const uint8_t slot, const uint32_t flag) {
  uint32_t value = rtcMemVeriRead(slot, 0);
  value |= flag;
  rtcMemVeriWrite(slot, value);
}

/**
 * Unset a flag to a verified RTC slot
 */
void rtcMemFlagUnset(const uint8_t slot, const uint32_t flag) {
  uint32_t value = rtcMemVeriRead(slot, 0);
  value &= ~flag;
  rtcMemVeriWrite(slot, value);
}

/**
 * Read a flag to a verified RTC slot
 */
uint32_t rtcMemFlagGet(const uint8_t slot, const uint32_t flag) {
  uint32_t value = rtcMemVeriRead(slot, 0);
  return value & flag;
}

/**
 * Invalidates an RTC slot
 */
void rtcMemInvalidate(const uint8_t slot) {
  uint32_t data = 0x00;
  ESP.rtcUserMemoryWrite(slot, &data, sizeof(data));
}

/**
 * Invalidates the entire RTC memory
 */
void rtcMemInvalidateAll() {
  for (uint8_t i=0; i<RTCMEM_MAX_SLOT; ++i) {
    rtcMemInvalidate(i);
  }
}

/**
 * Write arbitrary structures or values on the RTC memory (up to 255 bytes)
 *
 * Returns the number of bytes written or 0 in case of error.
 */
uint8_t rtcMemWrite(uint8_t slot, uint8_t offset, uint32_t &value) {
  ESP.rtcUserMemoryWrite(slot, &value, sizeof(uint32_t));
  return 4;
}
uint8_t rtcMemWrite(uint8_t slot, uint8_t offset, uint16_t &value) {
  uint32_t chunk, mask;
  if (offset > 1) return 0;

  // Update partial slot
  ESP.rtcUserMemoryRead(slot, &chunk, sizeof(chunk));
  mask = ~(0xFFFF << (16 * offset));
  chunk = (value << (16 * offset)) | (chunk & mask);
  ESP.rtcUserMemoryWrite(slot, &chunk, sizeof(uint32_t));

  return 2;
}
uint8_t rtcMemWrite(uint8_t slot, uint8_t offset, uint8_t &value) {
  uint32_t chunk, mask;
  if (offset > 3) return 0;

  // Update partial slot
  ESP.rtcUserMemoryRead(slot, &chunk, sizeof(chunk));
  mask = ~(0xFF << (8 * offset));
  chunk = (value << (8 * offset)) | (chunk & mask);
  ESP.rtcUserMemoryWrite(slot, &chunk, sizeof(uint32_t));

  return 1;
}

/**
 * Read arbitrary structures or values on the RTC memory (up to 255 bytes)
 *
 * Returns the number of bytes read or 0 in case of error.
 */
uint8_t rtcMemRead(const uint8_t slot, const uint8_t offset, uint32_t &value) {
  ESP.rtcUserMemoryRead(slot, &value, sizeof(uint32_t));
  return 4;
}
uint8_t rtcMemRead(const uint8_t slot, const uint8_t offset, uint16_t &value) {
  uint32_t chunk;
  if (offset > 1) return 0;

  // Update partial slot
  ESP.rtcUserMemoryRead(slot, &chunk, sizeof(chunk));
  value = (chunk >> (16 * offset)) && 0xFFFF;
  return 2;
}
uint8_t rtcMemRead(const uint8_t slot, const uint8_t offset, uint8_t &value) {
  uint32_t chunk;
  if (offset > 3) return 0;

  // Update partial slot
  ESP.rtcUserMemoryRead(slot, &chunk, sizeof(chunk));
  value = (chunk >> (8 * offset)) && 0xFF;

  return 1;
}
