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
#include <uNode.hpp>

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
    .mode = LORA_TTN_ABP,
    .activation = {
      .abp = {
        .appKey = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
        .netKey = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
        .devAddr = 0x00000000
      }
    }
  },
  .undervoltageProtection = true
};

/**
 * Helper function to shut down the chip when the packet is sent
 */
void packetSent(int status) {
  Serial.println("Packet Sent");

  // Go to sleep for 30 seconds
  // (Will reset the sketch when waking up from sleep)
  uNode.deepSleep(30);
}

/**
 * Sketch setup
 */
void setup() {
  uNode.setup();

  // Make the GPIO a general purpose input
  pinMode(D0, INPUT_PULLUP);

  // IMPORTANT : When you are testing this sketch, remember to connect D0 to
  //             the DTR pin instead of the GND. Connecting it to GND will make
  //             the device boot into an invalid mode! DTR is pulled LOW after
  //             boot and therefore behaves as a safer alternative to GND.

  // Send the value of the GPIO pin as a LoRaWAN packet
  if (digitalRead(D0) == HIGH) {
    uNode.sendLoRa("pin=1", 5, packetSent);
  } else {
    uNode.sendLoRa("pin=0", 5, packetSent);
  }
}

/**
 * Sketch loop
 */
void loop() {
  uNode.step();
}
