/*******************************************************************************
 * Copyright (c) 2015 Thomas Telkamp and Matthijs Kooijman
 * Copyright (c) 2018 Terry Moore, MCCI
 *
 * Permission is hereby granted, free of charge, to anyone
 * obtaining a copy of this document and accompanying files,
 * to do whatever they want with them without any restriction,
 * including, but not limited to, copying, modification and redistribution.
 * NO WARRANTY OF ANY KIND IS PROVIDED.
 *
 * This example sends a valid LoRaWAN packet with payload "Hello,
 * world!", using frequency and encryption settings matching those of
 * the The Things Network.
 *
 * This uses ABP (Activation-by-personalisation), where a DevAddr and
 * Session keys are preconfigured (unlike OTAA, where a DevEUI and
 * application key is configured, while the DevAddr and session keys are
 * assigned/generated in the over-the-air-activation procedure).
 *
 * Note: LoRaWAN per sub-band duty-cycle limitation is enforced (1% in
 * g1, 0.1% in g2), but not the TTN fair usage policy (which is probably
 * violated by this sketch when left running for longer)!
 *
 * To use this sketch, first register your application and device with
 * the things network, to set or generate a DevAddr, NwkSKey and
 * AppSKey. Each device should have their own unique values for these
 * fields.
 *
 * Do not forget to define the radio type correctly in config.h.
 *
 *******************************************************************************/

#include <Arduino.h>
#include "../../vendor/LMIC-Arduino/lmic.h"
#include "../../vendor/LMIC-Arduino/hal/hal.h"
#include <SPI.h>

#include "../util/SystemConfig.hpp"
#include "../util/RTCMem.hpp"
#include "../Pinout.hpp"
#include "LoRa.hpp"

#define DEBUG_CONTEXT "LoRa"
#include "../util/Debug.hpp"

/**
 * Initialize the singleton
 */
LoRaClass LoRa;

// Pin mapping for uNode
const lmic_pinmap lmic_pins = {
  .nss = UPIN_RFM_EN,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = LMIC_UNUSED_PIN,
  .dio = {UPIN_RFM_DIO0, UPIN_RFM_DIO1, LMIC_UNUSED_PIN},
};

/**
 * Structure for persisting the LoRa OTAA configuration
 */
struct OTAAPersistence {
  uint32_t  netid;
  uint32_t  devaddr;
  uint8_t   nwkKey[16];
  uint8_t   artKey[16];
  uint32_t  seqnoDn;
  uint32_t  seqnoUp;
} persistedConfig;

// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
// 0x70.
void os_getArtEui (u1_t* buf) {
  if (system_config.lora.mode == LORA_TTN_OTAA) {
    memcpy(buf, system_config.lora.activation.otaa.appEui, 8);
  }
}
void os_getDevEui (u1_t* buf) {
  if (system_config.lora.mode == LORA_TTN_OTAA) {
    memcpy(buf, system_config.lora.activation.otaa.devEui, 8);
  }
}
void os_getDevKey (u1_t* buf) {
  if (system_config.lora.mode == LORA_TTN_OTAA) {
    memcpy(buf, system_config.lora.activation.otaa.appKey, 16);
  }
}

/**
 * Handler for LMic
 */
void onEvent(ev_t ev) {
  switch(ev) {
    case EV_SCAN_TIMEOUT:
      logDebug("Scan Timeout");
      break;
    case EV_BEACON_FOUND:
      logDebug("Beacon Found");
      break;
    case EV_BEACON_MISSED:
      logDebug("Beacon Missed");
      break;
    case EV_BEACON_TRACKED:
      logDebug("Beacon Tracked");
      break;
    case EV_JOINING:
      logDebug("Joining");
      break;
    case EV_JOINED:
      logDebug("Joined");

      // Disable link check validation (automatically enabled
      // during join, but not supported by TTN at this time).
      LMIC_setLinkCheckMode(0);

      // Persist the OTAA configuration
      if (system_config.lora.mode == LORA_TTN_OTAA) {
        logDebug("Persisting OTAA session");

        // Take a snapshot of the session information
        persistedConfig.netid = LMIC.netid;
        persistedConfig.devaddr = LMIC.devaddr;
        persistedConfig.seqnoDn = LMIC.seqnoDn;
        persistedConfig.seqnoUp = LMIC.seqnoUp;
        memcpy(persistedConfig.nwkKey, LMIC.nwkKey, sizeof(persistedConfig.nwkKey));
        memcpy(persistedConfig.artKey, LMIC.artKey, sizeof(persistedConfig.artKey));

        // Persist state on the RTC memory (persisted across deep sleeps)
        if (rtcMemWrite(RTCMEM_SLOT_LORAPERSIST, 0, persistedConfig) != 0) {
          logDebug("Marking device as OTAA-Joined");
          rtcMemFlagSet(RTCMEM_SLOT_BOOTFLAGS, BOOTFLAG_LORA_JOINED);
        }
      }

      // If the user wants to know when we are joined, call-out now
      if (LoRa.joinedCb != NULL) {
        LoRa.joinedCb(1);
        LoRa.joinedCb = NULL;
      }
      break;
    case EV_JOIN_FAILED:
      logDebug("Join Failed");

      // If the user wants to know about join status, call-out now
      if (LoRa.joinedCb != NULL) {
        LoRa.joinedCb(0);
      }
      break;
    case EV_REJOIN_FAILED:
      logDebug("Rejoin Failed");

      // If the user wants to know about join status, call-out now
      if (LoRa.joinedCb != NULL) {
        LoRa.joinedCb(0);
      }
      break;
    case EV_TXCOMPLETE:
      logDebug("Tx Completed");
      if (LMIC.txrxFlags & TXRX_ACK)
        logDebug("Ack received");
      if (LMIC.dataLen) {
        logDebug("Received %d bytes", LMIC.dataLen);
      }

      // If we are using OTAA mode, it means we are using the RTC memory
      // to keep track of the current state. Now it's a good time to update the
      // frame counters.
      if (system_config.lora.mode == LORA_TTN_OTAA) {
        persistedConfig.seqnoDn = LMIC.seqnoDn;
        persistedConfig.seqnoUp = LMIC.seqnoUp;
        rtcMemWrite(RTCMEM_SLOT_LORAPERSIST, 0, persistedConfig);
      }

      // We managed to send some data, reset possible pending re-try
      if (LoRa.flags.pending) {
        LoRa.flags.pending = 0;
      }
      if (LoRa.loraCb != NULL) {
        if (LMIC.dataLen) {
          LoRa.loraCb(EV_TXCOMPLETE, &LMIC.frame[LMIC.dataBeg], LMIC.dataLen);
        } else {
          LoRa.loraCb(EV_TXCOMPLETE, NULL, 0);
        }
        LoRa.loraCb = NULL;
      }
      break;
    case EV_LOST_TSYNC:
      logDebug("Lost Sync");
      break;
    case EV_RESET:
      logDebug("Reset");
      break;
    case EV_RXCOMPLETE:
      // data received in ping slot
      logDebug("Rx Completed");

      // If we are using OTAA mode, it means we are using the RTC memory
      // to keep track of the current state. Now it's a good time to update the
      // frame counters.
      if (system_config.lora.mode == LORA_TTN_OTAA) {
        persistedConfig.seqnoDn = LMIC.seqnoDn;
        persistedConfig.seqnoUp = LMIC.seqnoUp;
        rtcMemWrite(RTCMEM_SLOT_LORAPERSIST, 0, persistedConfig);
      }

      break;
    case EV_LINK_DEAD:
      logDebug("Link Dead");
      break;
    case EV_LINK_ALIVE:
      logDebug("Link Alive");
      break;
    default:
      logDebug("Unknown event: %u", (unsigned)ev);
      break;
  }
}

/**
 * Initialize the LoRa subsystem, but not the chip itself
 */
void LoRaClass::begin() {
  loraCb = NULL;
  if (system_config.lora.mode == LORA_DISABLED) {
    return;
  }

  // LMIC init && reset MAC state (transfers are stopped)
  os_init();
  LMIC_reset();

  // Initialize LMIC in ABP Mode
  if (system_config.lora.mode == LORA_TTN_ABP) {

    // Start session
    LMIC_setSession(0x13, system_config.lora.activation.abp.devAddr,
                 (uint8_t*)system_config.lora.activation.abp.netKey,
                 (uint8_t*)system_config.lora.activation.abp.appKey);

    // Configure channels
    configureTTNChannels();

  }

  // Initialize LMIC in OTAA Mode
  if (system_config.lora.mode == LORA_TTN_OTAA) {

    // If we are starting in OTAA mode and we are already joined, re-use the
    // last saved information and resume the session.
    if (rtcMemFlagGet(RTCMEM_SLOT_BOOTFLAGS, BOOTFLAG_LORA_JOINED) != 0) {
      logDebug("The device is OTAA-Joined, reading session info");
      if (rtcMemRead(RTCMEM_SLOT_LORAPERSIST, 0, persistedConfig) != 0) {
        logDebug("Resuming OTAA session");

        // Resume session
        LMIC_setSession(persistedConfig.netid, persistedConfig.devaddr,
                     (uint8_t*)persistedConfig.nwkKey,
                     (uint8_t*)persistedConfig.artKey);

        // Restore frame counters
        LMIC.seqnoDn = persistedConfig.seqnoDn;
        LMIC.seqnoUp = persistedConfig.seqnoUp;

        // Configure channels
        configureTTNChannels();
      }
    }

  }


  // Enable loop
  flags.configured = 1;
  flags.pending = 0;
  logDebug("Ready");
}

/**
 * Soft-disable of the LoRa radio
 */
void LoRaClass::end() {
  if (system_config.lora.mode == LORA_DISABLED) {
    return;
  }

  flags.configured = 0;
  LMIC_shutdown();
  logDebug("Shut down");
}

/**
 * Handle LoRa chip events
 */
void LoRaClass::step() {
  if (system_config.lora.mode == LORA_DISABLED) {
    return;
  }

  if (!flags.configured) return;
  if (flags.pending && (millis() > pending.timeout_ts)) {
    pending.timeout_ts = millis() + pending.timeout;
    if (--pending.retries == 0) {
      // Check if we ran out of retries
      logDebug("Retries exceeded");
      if (loraCb != NULL) {
        loraCb(0, NULL, 0);
        loraCb = NULL;
      }
    } else {
      logDebug("Transmission timed out. Retrying...");
      sendRaw(pending.data, pending.len);
    }
  }

  // Handle LMIC events
  os_runloop_once();
}

/**
 * Send something over the radio
 *
 * Returns the numbers of bytes sent. 0 indicates an error.
 */
size_t LoRaClass::sendRaw(const char * data, size_t len) {
  if (system_config.lora.mode == LORA_DISABLED) {
    return 0;
  }

  if (LMIC.opmode & OP_TXRXPEND) {
    logDebug("Not sending %d bytes: pending Rx/Tx", len);
    return 0;
  }
  else {
    logDebug("Sending %d bytes", len);
    LMIC_setTxData2(1, (uint8_t*)data, len, 0);
    return len;
  }
}

/**
 * Send something, but keep checking if the transmission was successful
 */
void LoRaClass::sendManaged(const char * data, size_t len,
                   uint16_t retries, uint16_t timeout) {
  if (system_config.lora.mode == LORA_DISABLED) {
    return;
  }

  // Schedule managed transmission
  pending.data = data;
  pending.len = len;
  pending.retries = retries;
  pending.timeout = timeout;
  flags.pending = 1;

  // Schedule timeout
  pending.timeout_ts = millis() + timeout;

  // First attempt is asap
  sendRaw(data, len);
}

/**
 * Configure LMIC to use the pre-defined TTN channel configuration
 */
void LoRaClass::configureTTNChannels() {
  // Set up the channels used by the Things Network, which corresponds
  // to the defaults of most gateways. Without this, only three base
  // channels from the LoRaWAN specification are used, which certainly
  // works, so it is good for debugging, but can overload those
  // frequencies, so be sure to configure the full frequency range of
  // your network here (unless your network autoconfigures them).
  // Setting up channels should happen after LMIC_setSession, as that
  // configures the minimal channel set.

  LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI);      // g-band
  LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band

  // FSK is not possible with uNode v1.1, so the following line cannot be used:
  //LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK,  DR_FSK),  BAND_MILLI);      // g2-band
  // But the channel 8 has to be used, so we are repeating channel 0
  LMIC_disableChannel(8);

  // TTN defines an additional channel at 869.525Mhz using SF9 for class B
  // devices' ping slots. LMIC does not have an easy way to define set this
  // frequency and support for class B is spotty and untested, so this
  // frequency is not configured here.

  // Disable link check validation and ADR mode
  LMIC_setLinkCheckMode(0);
  LMIC_setAdrMode(system_config.lora.adr);

  // TTN uses SF9 for its RX2 window.
  LMIC.dn2Dr = DR_SF9;

  // Set data rate and transmit power for uplink
  LMIC_setDrTxpow(
    system_config.lora.tx_sf - 1,
    system_config.lora.tx_power
  );

  // Debug
  logDebug(
    "Configured with SF=#%d, PW=%d, ADR=%d",
    system_config.lora.tx_sf,
    system_config.lora.tx_power,
    system_config.lora.adr
  );
}

/**
 * Call the designated callback when a LoRa packet is sent
 */
void LoRaClass::whenSent(fnLoRaDataCallback cb) {
  loraCb = cb;
}

/**
 * Call the designated callback when a LoRa has joined over OTAA
 */
void LoRaClass::whenJoined(fnLoRaCallback cb) {
  joinedCb = cb;
}
