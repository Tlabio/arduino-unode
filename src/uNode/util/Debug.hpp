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
#ifndef DEBUG_H
#define DEBUG_H
#include "SystemConfig.hpp"

#define DEBUG_ENABLE

// Make sure a debug context is defined before including this file
#ifndef DEBUG_CONTEXT
#error "Debug context was not defined. Please set DEBUG_CONTEXT first"
#endif

// Macros that expand to debug logging
#ifdef DEBUG_ENABLE
  #define logDebug(message, ...) \
    if (system_config.logging.level >= LOG_LEVEL_INFO) { \
      Serial.printf("[" DEBUG_CONTEXT "] " message "\n", ##__VA_ARGS__); \
    }

// If debugging is globally disabled, provide stubs
#else
  #define logDebug(message) ;
#endif

#endif
