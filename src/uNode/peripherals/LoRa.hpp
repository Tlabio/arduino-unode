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
#ifndef LORA_TTN_H
#define LORA_TTN_H
#include "../Config.hpp"
#include "../PublicDefinitions.hpp"

/**
 * The LoRA class that interfaces with the LoRa chip
 */
class LoRaClass {
public:

  /**
   * Enable and configure the LoRa subsystem
   */
  void begin();

  /**
   * Stop an disable the LoRa subsystem
   */
  void end();

  /**
   * Called periodically to process the LoRA events
   */
  void step();

  /**
   * Send something over the radio
   *
   * Returns the numbers of bytes sent. 0 indicates an error.
   */
  size_t sendRaw(const char * data, size_t len);

  /**
   * Send something, but manage the transmission and if it's not sent re-try
   *
   * /!\ It's the user's responsibility to preserve the contents of the `data`
   *     pointer until the transmission is completed!
   *
   * Returns the numbers of bytes sent. 0 indicates an error.
   */
  void sendManaged(const char * data, size_t len,
                   uint16_t retries = 10, uint16_t timeout = 10000);

  /**
   * Call the designated callback when a LoRa packet is sent
   */
  void whenSent(fnLoRaCallback cb);

  /**
   * Call the designated callback when a LoRa has joined the network
   */
  void whenJoined(fnLoRaCallback cb);

  /**
   * Configure LMIC to use the pre-defined TTN channel configuration
   */
  void configureTTNChannels();


  struct {

    /**
     * This flag denotes that `.begin` is called
     */
    uint8_t   configured : 1;

    /**
     * Pending transmission in the data buffer
     */
    uint8_t   pending : 1;

  } flags;

  /**
   * The user callback to call when a packet is sent
   */
  fnLoRaCallback loraCb;

  /**
   * The user callback to call when LoRa has joined a network
   */
  fnLoRaCallback joinedCb;

  /**
   * Pending transmission
   */
  struct {
    const char * data;
    size_t len;
    unsigned long timeout_ts;
    uint16_t timeout;
    uint8_t retries;
  } pending;

};

/**
 * Singleton of the LoRaClass, available as `LoRa`
 */
extern LoRaClass LoRa;

#endif
