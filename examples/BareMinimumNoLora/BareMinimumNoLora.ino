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

 /******************************************************************************
  * This scetch is the bare minimum sketch with:
  *  - LoRa switched off
  *  - Under voltage protection switched on
  *  - Async port initialised to 115200 b/s with default logging on
  * and is does nothing usefull. That is for you to add...
  *
  *  The board set up should be:
  *     Generic ESP8266 module
  *     Flash Mode = DIO
  *     Flash Size = Select a 4 MB option. Remember you need twice the space for OTA
  *     And your port, either on the network or on serial
  *
  * Contributed 13/01/2019, Gijs Mos, Sensemakers Amsterdam, www.sensemakersams.org
  */
#include <uNodeOpen.hpp>

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
    .mode = LORA_DISABLED           // Disable LoRa. Consult TTN examples to switch LoRa on.
  },
  .logging = LOG_DEFAULT            // Or use LOG_DISABLED or LOG_INFO
};

/**
 * Sketch setup
 */
void setup() {
  uNode.setup();                    // Initialize the uNode library
  // Put your own setup here.
  // And remember to use uNode.portMode() and uNode.digitalXXX() if you want the GPIO extensions.
}

/**
 * Sketch loop
 */
void loop() {
  uNode.step();                      // Run as frequently as possible to keep background processing going.
  // Put your own loop code here.  And if you have long running stuff, don't forget to call uNode.step() often.
  // And also remember to use uNode.portMode() and uNode.digitalXXX() if you want the GPIO extensions.
}
