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
#ifndef GPIO_H
#define GPIO_H
#include <SPI.h>

#include "../../vendor/mcp23s08/mcp23s08.h"

/**
 * Shorthand to the GPIO expansion interface
 */
class GPIOClass {
public:

  /**
   * Constructor
   */
  GPIOClass();

  /**
   * Enable the GPIO expansion chip
   */
  void begin();

  /**
   * Disable the GPIO expansion chip
   */
  void end();

  /**
   * Set the pin direction on the GPIO chip
   */
  void pinMode(uint8_t pin, uint8_t mode);

  /**
   * Write a logical value on the pin
   */
  void digitalWrite(uint8_t pin, uint8_t value);

  /**
   * Read a logical value on the pin
   */
  int digitalRead(uint8_t pin);

private:
  mcp23s08 mcp;

};

/**
 * Singleton of the GPIOClass, available as `GPIO`
 */
extern GPIOClass GPIO;

#endif
