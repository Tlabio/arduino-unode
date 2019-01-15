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
#include "Checksums.hpp"

/**
 * The CRC4 lookup table
 */
static const uint8_t crc4_tab[] = {
  0x0, 0x7, 0xe, 0x9, 0xb, 0xc, 0x5, 0x2,
  0x1, 0x6, 0xf, 0x8, 0xa, 0xd, 0x4, 0x3,
};

/**
 * crc4 - calculate the 4-bit crc of a value.
 * @c:    starting crc4
 * @x:    value to checksum
 * @bits: number of bits in @x to checksum
 *
 * Returns the crc4 value of @x, using polynomial 0b10111.
 *
 * The @x value is treated as left-aligned, and bits above @bits are ignored
 * in the crc calculations.
 */
uint8_t crc4(uint8_t c, uint32_t x, int bits)
{
  int i;

  /* mask off anything above the top bit */
  x &= (1ull << bits) - 1;

  /* Align to 4-bits */
  bits = (bits + 3) & ~0x3;

  /* Calculate crc4 over four-bit nibbles, starting at the MSbit */
  for (i = bits - 4; i >= 0; i -= 4)
    c = crc4_tab[c ^ ((x >> i) & 0xf)];

  return c;
}

/**
 * CRC16 Checksum Calculation
 */
uint16_t crc16 (uint8_t* data, uint32_t len, uint16_t polynomial) {
    uint16_t remainder = 0;
    for( uint32_t i = 0; i < len; i++ ) {
        remainder ^= data[i] << 8;
        for( uint8_t bit = 8; bit > 0; bit--) {
            if( (remainder & 0x8000) )
                remainder = (remainder << 1) ^ polynomial;
            else
                remainder <<= 1;
        }
    }
    return remainder;
}
