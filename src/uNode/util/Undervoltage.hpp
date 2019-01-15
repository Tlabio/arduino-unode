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
#ifndef UNDERVOLTAGE_H
#define UNDERVOLTAGE_H

/**
 * Checks the undervoltage lockdown
 *
 * This should be called as early as possible in the boot sequence. It checks if
 * there is an active under-voltage lockdown and if the voltage is high enough
 * to recover.
 */
void undervoltageCheckLockdown();

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
void undervoltageProtect();

#endif
