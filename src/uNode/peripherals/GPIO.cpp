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

// 13/01/2019. Gijs Mos. GPIOClass::pinMode handles bad modes now consistently and safely.
// 14/01/2019. Gijs Mos. Pulled in SPI code. mcp23s08 library no longer required.

#include <Arduino.h>
#include "GPIO.hpp"
#include "../Pinout.hpp"

#define pinMask(pin)  (1 << pin)

#define MCP23S08_HAEN_SHIFT 3
#define MCP23S08_SEQOP_SHIFT 5


/**
 * Initialize the singleton
 */
GPIOClass GPIO;


/**
 * Constructor
 */
GPIOClass::GPIOClass() {
}


/**
 * Enable the GPIO expansion chip
 */
void GPIOClass::begin() {

  SPI.begin();                // Setup the SPI.

  ::pinMode( UPIN_GPIO, OUTPUT );
  ::digitalWrite( UPIN_GPIO, HIGH );      // GPIO Chip Select not selected.

  // Enable hardware address pins and disable auto register address increment.
  writeRegister( registerIOCON, ( 1 << MCP23S08_HAEN_SHIFT ) | ( 1 << MCP23S08_SEQOP_SHIFT ) );

  // make sure shadow registers are in sync with chip (same as PU reset state).
  _inputPullup = 0;             // Inputs in high Z.
  writeRegister( registerGPPU, _inputPullup );

  _direction = 0xff;              // All input
  writeRegister( registerIODIR, _direction );

  _outputState = 0;             // And outputs starting low
  writeRegister( registerGPIO, _outputState );
}


/**
 * Disable the GPIO expansion chip
 */
void GPIOClass::end() {
  // Nothing to do here.
  // Cannot end SPI because there are other chips on SPI.
  // [TBD] Perhaps we should put the whole thing in High Z.
}


/**
 * Set the pin direction on the GPIO chip
 */
void GPIOClass::pinMode( uint8_t pin, uint8_t mode ) {
  if ( pin > 7 ) return;            // Silently ignore non-existing pins.

  uint8_t direction = _direction;
  uint8_t pullup = _inputPullup;

  switch ( mode ) {
    case OUTPUT:
      _direction &= ~pinMask( pin );
      break;

    case INPUT_PULLUP:
      _inputPullup |= pinMask( pin );
      _direction |= pinMask( pin );
      break;

    case INPUT:
    default:                  // Least harmfull one if unknown mode
      _inputPullup &= ~pinMask( pin );
      _direction |= pinMask( pin );
      break;
  }

  if ( pullup != _inputPullup ) writeRegister( registerGPPU, _inputPullup );
  if ( direction != _direction ) writeRegister( registerIODIR, _direction );
}


/**
 * Write a logical value on the pin
 */
void GPIOClass::digitalWrite( uint8_t pin, uint8_t value ) {
  if ( pin > 7 ) return;            // Silently ignore non-existing pins.
  if ( value ) {
    if ( _outputState & pinMask( pin ) ) return;  // Already on.
    _outputState |= pinMask( pin );
  } else {
    if ( ( _outputState & pinMask( pin ) ) == 0 ) return; // Already off.
    _outputState &= ~pinMask( pin );
  }
  writeRegister( registerGPIO, _outputState );
}

/**
 * Read a logical value from the pin
 */
uint8_t GPIOClass::digitalRead( uint8_t pin ) {
  if ( pin > 7 ) return LOW;          // Silently ignore non-existing pins, returning LOW.
  return ( readRegister( registerGPIO ) & pinMask( pin ) ? HIGH : LOW );
}

/**
 * Measure half pulse time (upto a maximum) on a pin.
 */
int16_t GPIOClass::timeHalfPulse( uint8_t pin ) {
  uint32_t startTime = micros();
  uint8_t ourPinMask = pinMask( pin );
  uint16_t waitingTime;

  startSPI();
  (void) SPI.transfer( opcodeReadGPIO );
  (void) SPI.transfer( registerGPIO );
  uint8_t startState = SPI.transfer( 0 ) & ourPinMask;  // Get initial state at startTime

  // Since we may be called with interrupts off we will allow maximum of "maximumHalfPulseLength" microseconds
  while ( ( waitingTime = micros() - startTime ) <= maximumHalfPulseLength ) {
    // We can use repeated read because in setup we disabled auto register address increment.
    if ( ( SPI.transfer( 0 ) & ourPinMask ) == startState ) continue;  // Not yet, try harder ;-)
    stopSPI();
    return waitingTime;
  }
  // Time out
  stopSPI();
  return -1;
}


/**
 * Write "value" to MCP23S08 register "reg".
 */
void GPIOClass::writeRegister( uint8_t reg, uint8_t value ) {
  startSPI();

  (void) SPI.transfer( opcodeWriteGPIO );
  (void) SPI.transfer( reg );
  (void) SPI.transfer( value );

  stopSPI();
}


/**
 * Read value of MCP23S08 register "reg".
 */
uint8_t GPIOClass::readRegister( uint8_t reg ) {
  uint8_t result;

  startSPI();

  (void) SPI.transfer( opcodeReadGPIO );
  (void) SPI.transfer( reg );
  result = SPI.transfer( 0 );

  stopSPI();
  return result;
}


/**
 * Prepare SPI bus and select the MCP23S08,
 */
void GPIOClass::startSPI() {
  SPI.beginTransaction( SPISettings( 12000000, MSBFIRST, SPI_MODE0 ) );
  ::digitalWrite( UPIN_GPIO, LOW );
}


/**
 * Deselect the MCP23S08, and free the SPI bus.
 */
void GPIOClass::stopSPI() {
  ::digitalWrite( UPIN_GPIO, HIGH );
  SPI.endTransaction();
}
