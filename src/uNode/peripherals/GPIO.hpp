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

#define MCP23S08_ADDRESS 0x21

/**
   Shorthand to the GPIO expansion interface
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
    void pinMode( uint8_t pin, uint8_t mode );

    /**
     * Write a logical value on the pin
     */
    void digitalWrite( uint8_t pin, uint8_t value );

    /**
     * Read a logical value on the pin
     */
    uint8_t digitalRead( uint8_t pin );


    /**
     * Time a half pulse in Micros
     */
    static const uint16_t maximumHalfPulseLength = 500;  // micro seconds
    int16_t timeHalfPulse( uint8_t pin );

  private:

    // Master (shadow registers)
    uint8_t _direction;        // registerIODIR content
    uint8_t _inputPullup;       // registerGPPU content
    uint8_t _outputState;       // registerOLAT content

    // SPI operation codes and address
    const uint8_t opcodeReadGPIO = ( MCP23S08_ADDRESS << 1 ) | 1;
    const uint8_t opcodeWriteGPIO = ( MCP23S08_ADDRESS << 1 );

    // Registers in MCP23S08
    const uint8_t registerIODIR = 0x00;     // IO Direction
    const uint8_t registerIOCON = 0x05;     // IO Control
    const uint8_t registerGPPU  = 0x06;     // Input pullups
    const uint8_t registerGPIO  = 0x09;     // Port data (read INPUT)

    void writeRegister( uint8_t reg, uint8_t value );
    uint8_t readRegister( uint8_t reg );
    void startSPI();
    void stopSPI();

};

/**
   Singleton of the GPIOClass, available as `GPIO`
*/
extern GPIOClass GPIO;

#endif
