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
#ifndef CHECKSUMS_H
#define CHECKSUMS_H

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
uint8_t crc4(uint8_t c, uint32_t x, int bits);

/**
 * crc16 - calculate the 16-bit crc of the given value
 */
uint16_t crc16(uint8_t* data, uint32_t len, uint16_t polynomial = 0x1021);

#endif
