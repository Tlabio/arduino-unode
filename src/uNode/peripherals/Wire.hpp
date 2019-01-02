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
#ifndef WIRE_H
#define WIRE_H

// Prohibit ESP8266 implementation of `Wire.h` to define the global Wire instance.
// We are going to provide our own alternative.
#define NO_GLOBAL_TWOWIRE

#include "../../vendor/SoftWire/SoftWire.h"

/**
 * A SoftWire implementation using the seamless GPIO expansion middleware
 */
class GPIOSoftWire: public SoftWire {
public:

  /**
   * Default factory for the SoftWire singleton
   */
  GPIOSoftWire(): SoftWire(0,0) { };

  /**
   * Begin methods with and without pin configuration
   */
  void begin(uint8_t sda, uint8_t scl);
  void begin(void);

};

/**
 * Singleton of the SoftWire class, available as `Wire`
 */
extern GPIOSoftWire Wire;

#endif
