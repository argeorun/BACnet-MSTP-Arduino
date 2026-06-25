/**
 * @file rs485_port_config.h
 * @brief RS-485 HardwareSerial port selection
 * @author George Arun
 * @date 2025-2026
 * @copyright SPDX-License-Identifier: MIT
 */
// Optional: define RS485_PORT here to select the HardwareSerial used for RS485.
// Default for Arduino Mega 2560: use Serial1 (TX1=18, RX1=19).
#ifndef RS485_PORT

#if defined(ARDUINO_ARCH_STM32)

HardwareSerial Serial6(PG9,PG14);
#define RS485_PORT Serial6

#elif defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_IDF_TARGET_ESP32S3)
/* ESP32-S3: Serial2 on GPIO17(TX)/GPIO16(RX) */
#define RS485_PORT Serial2

#elif defined(ARDUINO_ARCH_ESP32)
/* Generic ESP32: Serial2 on GPIO17(TX)/GPIO16(RX) — frees Serial for USB debug */
#define RS485_PORT Serial2

#elif defined(ARDUINO_AVR_MEGA2560)
/* Mega 2560: Serial1 on TX1=18, RX1=19 — frees Serial for USB debug */
#define RS485_PORT Serial1

#else
/* UNO and other single-UART AVR: Serial is the only option */
#define RS485_PORT Serial

#endif
#endif

// RS485_PORT defaults to Serial1 if not overridden by build flags or prior definitions.

/* =========================================================================
 * ESP32 UART pin overrides
 * Define these before including this header to remap TX/RX pins without
 * editing library source files.
 *
 * Example (in sketch, before including rs485_port_config.h):
 *   #define RS485_UART_TX_PIN  17
 *   #define RS485_UART_RX_PIN  16
 * ========================================================================= */
#if defined(ARDUINO_ARCH_ESP32)
#  ifndef RS485_UART_TX_PIN
#    define RS485_UART_TX_PIN  PIN_UART2_TX
#  endif
#  ifndef RS485_UART_RX_PIN
#    define RS485_UART_RX_PIN  PIN_UART2_RX
#  endif
#endif
