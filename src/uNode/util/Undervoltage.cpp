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
#include "SystemConfig.hpp"
#include "Undervoltage.hpp"
#include "RTCMem.hpp"

/**
 * Checks the undervoltage lockdown
 *
 * This should be called as early as possible in the boot sequence. It checks if
 * there is an active under-voltage lockdown and if the voltage is high enough
 * to recover.
 */
void undervoltageCheckLockdown() {
  uint8_t bootflags = rtcMemVeriRead(RTCMEM_SLOT_BOOTFLAGS);

  // If the system was shut down because of an undervoltage event, do not power
  // back again until the voltage raises above `enableThreshold` (or a normal
  // hardware reset takes place)
  if ((bootflags & BOOTFLAG_UNDERVOLTAGE_PROTECTION) != 0) {

    // If the threshold is not met, go back to sleep
    if (ESP.getVcc() < system_config.undervoltageProtection.enableThreshold) {
      ESP.deepSleep(1800 * 1e6, WAKE_RF_DISABLED);
      return;
    }

    // If the threshold is met the device can wake up. However, since we
    // have just rebooted from deep sleep with RF disabled, the board won't have
    // any RF capabilities. Therefore we should reset the UV protection flag
    // and reboot again after deep sleep with the RF enabled.
    bootflags &= ~RTCMEM_SLOT_BOOTFLAGS;
    rtcMemVeriWrite(RTCMEM_SLOT_BOOTFLAGS, bootflags);
    ESP.deepSleep(100, WAKE_RF_DEFAULT);

  }

}

/**
 * Checks for undervoltage condition in the system
 *
 * If the voltage is lower than `disableThreshold` the board will boot into the
 * lowest power mode possible and go right back to sleep ASAP. This will reduce
 * the power consumption to the bare minimum, enabling protection of the battery
 * and the rest of the system.
 *
 * If the voltage is higher than `enableThreshold`, the power will be enabled
 * back again and normal operation will resume.
 */
void undervoltageProtect() {
  // If the voltage enter into under-voltage shutdown
  if (ESP.getVcc() < system_config.undervoltageProtection.disableThreshold) {
    uint8_t bootflags = rtcMemVeriRead(RTCMEM_SLOT_BOOTFLAGS);

    // Make sure we set the undervoltage protection boot flag set
    if ((bootflags & BOOTFLAG_UNDERVOLTAGE_PROTECTION) == 0) {
      bootflags |= BOOTFLAG_UNDERVOLTAGE_PROTECTION;
      rtcMemVeriWrite(RTCMEM_SLOT_BOOTFLAGS, bootflags);
    }

    // Sleep for the maximum number of time that we can sleep. And when we
    // come back to life make sure we don't cause any spike that could take the
    // life of the battery down quicker.
    ESP.deepSleep(1800 * 1e6, WAKE_RF_DISABLED);
    return;
  }

  // The device is operating on the correct voltage range
}
