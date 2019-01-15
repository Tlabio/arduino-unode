/*******************************************************************************
 * Copyright (c) 2018 Ioannis Charalampidis - TLab.gr
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
#include <uNodeOpen.hpp>

// Include <Wire.h> *BEFORE* the other libraries that use it
#include <Wire.h>

/**
 * We are using the ADC to measure the battery voltage. If you are using the ADC
 * in your project, comment-out the following line.
 */
ADC_MODE(ADC_VCC);

/**
 * uNode library configuration
 */
uNodeConfig unode_config = {
  .lora = {
    .mode = LORA_DISABLED
  }
};

/**
 * Sketch setup
 */
void setup() {
  uNode.setup();

  // Start the SoftWire library on the GPIO expansion chip
  Wire.begin(
    D5, // SDA pin on the GPIO expansion
    D6  // SCL pin on the GPIO expansion
  );

  // You can use the software-Wire alternative as usual
  // (Libraries that use the Wire instance behind the scenes will automatically
  // use this version instead)
  Wire.beginTransmission(0x1234);
  Wire.write(0x1234);
  Wire.endTransmission();
}

/**
 * Sketch loop
 */
void loop() {
  uNode.step();
}
