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

/**
 * Checks if a triple-copy checksum is valid
 */
bool csum_is_valid(uint32_t & data) {
  uint8_t * copies = (uint8_t*)&data;
  if (copies[1] != (copies[0] + 2)) return false;
  if (copies[2] != (copies[0] * 5)) return false;
  if (copies[3] != (255 - copies[0])) return false;
  return true;
}

/**
 * Returns the value from a triple-copy structure
 */
uint8_t csum_get(uint32_t & data) {
  if (!csum_is_valid(data)) return 0;
  uint8_t * copies = (uint8_t*)&data;
  return copies[0];
}

/**
 * Sets the value to a triple-copy structure
 */
void csum_set(uint32_t & data, uint8_t value) {
  uint8_t * copies = (uint8_t*)&data;
  copies[0] = value;
  copies[1] = value + 2;
  copies[2] = value * 5;
  copies[3] = 255 - value;
}

/**
 * Increments the value of the triple-copy structure by one. If the value
 * is not valid, it assumes it to be 0.
 */
uint8_t csum_inc(uint32_t & data) {
  uint8_t value = csum_get(data) + 1;
  csum_set(data, value);
  return value;
}

/**
 * Read a verified value from the RTC memory
 */
uint8_t rtcMemVeriRead(const uint8_t slot, const uint8_t defaultValue) {
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
void rtcMemVeriWrite(const uint8_t slot, const uint8_t value) {
  uint32_t data;

  // Define the value including checksum guards
  csum_set(data, value);

  // Write memory
  ESP.rtcUserMemoryWrite(slot, &data, sizeof(data));
}

/**
 * Set a flag to a verified RTC slot
 */
void rtcMemFlagSet(const uint8_t slot, const uint8_t flag) {
  uint8_t value = rtcMemVeriRead(slot, 0);
  value |= flag;
  rtcMemVeriWrite(slot, value);
}

/**
 * Unset a flag to a verified RTC slot
 */
void rtcMemFlagUnset(const uint8_t slot, const uint8_t flag) {
  uint8_t value = rtcMemVeriRead(slot, 0);
  value &= ~flag;
  rtcMemVeriWrite(slot, value);
}

/**
 * Read a flag to a verified RTC slot
 */
uint8_t rtcMemFlagGet(const uint8_t slot, const uint8_t flag) {
  uint8_t value = rtcMemVeriRead(slot, 0);
  return value & flag;
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
