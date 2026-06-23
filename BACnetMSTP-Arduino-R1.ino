/**
 * @file BACnet-Arduino-working.ino
 * @brief Arduino BACnet MS/TP Device — sketch entry point (setup, loop, EEPROM bridge)
 * @author George Arun
 * @date 2025-2026
 * @copyright SPDX-License-Identifier: MIT
 */

#include <stdbool.h>
#include <stdint.h>

#include "src/compile_config.h"

#if defined(ARDUINO)
#include <Arduino.h>
#endif

#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_AVR)
#include <EEPROM.h>
#endif
/* ========================================================= */
/* EEPROM functions MUST use C linkage (required by nvdata) */
/* ========================================================= */

#if defined(ARDUINO_ARCH_STM32)
#include <FlashStorage_STM32.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

int eeprom_bytes_read(uint16_t eeaddr, uint8_t *buf, int len);
int eeprom_bytes_write(uint16_t eeaddr, const uint8_t *buf, int len);

#ifdef __cplusplus
}
#endif

  int eeprom_bytes_read(uint16_t eeaddr, uint8_t *buf, int len) {
    int count = 0;

    while (len--) {
      buf[count++] = EEPROM.read(eeaddr++);
    }

    return count;
  }

  int eeprom_bytes_write(uint16_t eeaddr, const uint8_t *buf, int len) {
    int count = 0;

    while (len-- && eeaddr < EEPROM.length()) {
      EEPROM.write(eeaddr++, buf[count++]);
    }

    return count;
  }




/* ========================================================= */
/* BACnet includes MUST use C linkage */
/* ========================================================= */

#include "src/platform/adc.h"
#include "src/platform/nvdata.h"
#include "src/platform/rs485.h"
#include "src/platform/stack.h"

#include "src/bacnet/datalink/datalink.h"
#include "src/bacnet/datalink/dlmstp.h"
#include "src/bacnet/npdu.h"
#include "src/bacnet/dcc.h"
#include "src/bacnet/basic/services.h"
#include "src/bacnet/basic/sys/mstimer.h"
#include "src/bacnet/basic/tsm/tsm.h"
#include "src/bacnet/basic/object/device.h"
#include "src/bacnet/basic/object/av.h"
#include "src/bacnet/basic/object/bv.h"


/* ========================================================= */
/* Globals */
/* ========================================================= */

extern bool Send_I_Am_Flag;

static struct mstimer Task_Timer;
static uint32_t Uptime_Seconds = 0;

static uint8_t PDUBuffer[MAX_MPDU + 16];


/* ========================================================= */
/* USER CONFIG */
/* ========================================================= */

#define AV_UPTIME_INSTANCE 0

/* ========================================================= */
/* Hardware init */
/* ========================================================= */

static void hardware_init(void) {
  RS485_Initialize();

  mstimer_init();

  adc_init();

#if defined(ARDUINO)
  interrupts();
#endif
}


/* ========================================================= */
/* 1 second task */
/* ========================================================= */

static void one_second_task(void) {
  float hours;

  Uptime_Seconds++;

  hours = (float)Uptime_Seconds / 3600.0f;

  Analog_Value_Present_Value_Set(
    AV_UPTIME_INSTANCE,
    hours,
    8);
}


/* ========================================================= */
/* NV data init */
/* ========================================================= */

static void device_nvdata_init(void) {
  uint8_t value8;
  uint16_t value16;
  uint32_t value32;

  value16 = nvdata_unsigned16(NV_EEPROM_TYPE_0);

  if (value16 != NV_EEPROM_TYPE_ID) {
    nvdata_unsigned16_set(NV_EEPROM_TYPE_0, NV_EEPROM_TYPE_ID);

    nvdata_unsigned8_set(NV_EEPROM_VERSION, NV_EEPROM_VERSION_ID);

    nvdata_unsigned8_set(NV_EEPROM_MSTP_MAC, 10);

    nvdata_unsigned8_set(NV_EEPROM_MSTP_BAUD_K, 38);

    nvdata_unsigned8_set(NV_EEPROM_MSTP_MAX_MASTER, 127);

    nvdata_unsigned24_set(NV_EEPROM_DEVICE_0, 260123);
  }

  value8 = nvdata_unsigned8(NV_EEPROM_MSTP_MAC);

  dlmstp_set_mac_address(value8);

  value8 = nvdata_unsigned8(NV_EEPROM_MSTP_BAUD_K);

  value32 = RS485_Baud_Rate_From_Kilo(value8);

  RS485_Set_Baud_Rate(value32);

  value8 = nvdata_unsigned8(NV_EEPROM_MSTP_MAX_MASTER);

  dlmstp_set_max_master(value8);

  dlmstp_set_max_info_frames(1);

  Send_I_Am_Flag = true;
}

/* ========================================================= */
/* Arduino setup */
/* ========================================================= */

void setup() {

  hardware_init();

  Analog_Value_Init();

  Binary_Value_Init();

  device_nvdata_init();

  dlmstp_init(NULL);

  mstimer_set(&Task_Timer, 1000);
}


/* ========================================================= */
/* Arduino loop */
/* ========================================================= */

void loop() {
  uint16_t pdu_len;

  BACNET_ADDRESS src;


  /* 1 second task */
  if (mstimer_expired(&Task_Timer)) {
    mstimer_reset(&Task_Timer);

    one_second_task();
  }


  /* BACnet receive */

  pdu_len = dlmstp_receive(&src, PDUBuffer, sizeof(PDUBuffer), 0);

  if (pdu_len) {
    npdu_handler(&src, PDUBuffer, pdu_len);
  }
}

