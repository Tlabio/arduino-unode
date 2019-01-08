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
#include "Health.hpp"
extern "C" {
  #include "user_interface.h"
  extern struct rst_info resetInfo;
}

/**
 * Integrity checks
 */
static_assert(sizeof(system_health_t) == 2, "Invalid size of system_health_t");

/**
 * A static structure that is re-used by the `system_health_get` function
 */
system_health_t _systemHealth;

/**
 * Initialize the system health structure
 */
void system_health_setup() {
  if (resetInfo.reason == REASON_DEFAULT_RST) { // normal startup by power on
    _systemHealth.reason = 0;
  } else if (resetInfo.reason == REASON_WDT_RST) { // hardware watch dog reset
    _systemHealth.reason = 1;
  } else if (resetInfo.reason == REASON_EXCEPTION_RST) { // exception reset, GPIO status won’t change
    _systemHealth.reason = 2;
  } else if (resetInfo.reason == REASON_SOFT_WDT_RST) { // software watch dog reset, GPIO status won’t change
    _systemHealth.reason = 3;
  } else if (resetInfo.reason == REASON_SOFT_RESTART) { // software restart ,system_restart , GPIO status won’t change
    _systemHealth.reason = 4;
  } else if (resetInfo.reason == REASON_DEEP_SLEEP_AWAKE) { // wake up from deep-sleep
    _systemHealth.reason = 5;
  } else if (resetInfo.reason == REASON_EXT_SYS_RST) { // external system reset
    _systemHealth.reason = 6;
  } else {
    _systemHealth.reason = 7;
  }
}

/**
 * Return the current system health status
 */
system_health_t * system_health_get() {
  uint8_t error = system_health_check();
  if (error != 0) {
    _systemHealth.reason = error;
    _systemHealth.error = 1;
  } else {
    // (Reason cannot be re-defined, since `resetInfo` is damaged after the
    //  sketch has started)
    _systemHealth.error = 0;
  }
  _systemHealth.vcc = ESP.getVcc();
  return &_systemHealth;
}

/**
 * User-overridable system health-check function
 *
 * If this function returns `0`, then the system is assumed to be healthy.
 * Otherwise a 3-bit error code should be returned (1 to 7), indicating the
 * reason of the error.
 */
uint8_t __attribute__((weak)) system_health_check() {
  return 0;
}
