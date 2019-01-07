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
#ifndef UNODE_H
#define UNODE_H

#include <Arduino.h>

// Dependencies that are going to be resolved from platformio
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>

// Bare minimum API exposed to the users
#include "uNode/Config.hpp"
#include "uNode/Pinout.hpp"
#include "uNode/PublicDefinitions.hpp"
#include "uNode/peripherals/Wire.hpp"

/**
 * The user-facing interface
 */
class uNodeClassOpen {
public:

  /**
   * Initialize the micro-node hardware
   */
  void setup();

  /**
   * Update micro-node interfaces
   */
  void step();

  /**
   * Send a packet over the LoRa network
   */
  void sendLoRa(const char * data, size_t size, fnLoRaDataCallback whenDone = nullptr);

  /**
   * Send a structure over LoRa
   */
  template <typename T>
  void sendLoRa(const T& data, fnLoRaDataCallback whenDone = nullptr) {
    this->sendLoRa(
      static_cast<const char*>(static_cast<const void*>(&data)),
      sizeof(T),
      whenDone
    );
  }

  /**
   * Send a structure over LoRa
   */
  template <typename T>
  void sendLoRa(const T* data, fnLoRaDataCallback whenDone = nullptr) {
    this->sendLoRa(
      static_cast<const char*>(static_cast<const void*>(data)),
      sizeof(T),
      whenDone
    );
  }

  /**
   * Connect to the access point with the given name
   *
   * This function blocks  until it's connected to the access point.
   */
  int connectWiFi(char * name, char * password, uint16_t timeout = 0);

  /**
   * Connect to any of the APs in the list
   */
  int connectWiFi(char ** names, char ** password, uint8_t size, uint16_t timeout = 0);

  /**
   * Send a packet over UDP
   */
  int sendUDP(char * data, size_t size, char * host, uint16_t port);

  /**
   * Send a packet over TCP
   */
  int sendTCP(char * data, size_t size, char * host, uint16_t port, uint16_t timeout = 0);

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

  /**
   * Put all or some peripherals on standby
   */
  void standby(STANDBY_MODE_t mode = STANDBY_ALL);

  /**
   * Enter deep sleep for the designated number of seconds
   */
  void deepSleep(const uint16_t seconds);

  /**
   * Enable or Disable the VBus explicitly
   */
  void enablePeripherals(const uint8_t enabled = 1);

  /**
   * Blink the built-in ESP8266 LED
   *
   * /!\ WARNING: This will make the `UPIN_GPIO` an output and blink the LED
   *              make sure you are not using that pin for other uses!
   */
  void blink(const uint16_t on_ms = 500, const uint16_t off_ms = 0, const uint8_t cycles = 1);

};

/**
 * Singleton of the uNodeClassOpen, available as `uNode`
 */
#ifndef __UNODE_NO_GLOBALS
extern uNodeClassOpen uNode;
#endif

#endif
