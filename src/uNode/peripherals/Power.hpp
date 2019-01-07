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
#ifndef POWER_H
#define POWER_H

/**
 * The power class is responsible for the power management on the device
 */
class PowerClass {
public:

  /**
   * Configure the power system
   */
  void begin();

  /**
   * Enable/Disable the power to the GPIO module
   */
  void setGPIO(uint8_t enabled);
  uint8_t getGPIO();

  /**
   * Enable/Disable LoRA
   */
  void setLoRaRadio(uint8_t enabled);
  uint8_t getLoRaRadio();

  /**
   * Enable/Disable WiFi Radio
   */
  void setWiFiRadio(uint8_t mode);
  uint8_t getWiFiRadio();

  /**
   * Enable/Disable VBus explicitly
   */
  void setVBusOverride(uint8_t enabled);
  uint8_t getVBusOverride();

  /**
   * Disable all peripherals
   */
  void off();

private:

  /**
   * Apply the state of the peripherals
   */
  void apply();

  /**
   * The state of the peripherals
   */
  struct {
    uint8_t   vbus: 1;
    uint8_t   lora: 1;
    uint8_t   gpio: 1;
    uint8_t   wifi: 2;
    uint8_t   ovrd: 1;
    uint8_t   _unused : 2;
  } state;

};

/**
 * Singleton of the PowerClass, available as `Power`
 */
extern PowerClass Power;

#endif
