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

#define RS485_PORT Serial2

#else

#define RS485_PORT Serial

#endif
#endif

/* GAP5: Overridable UART pin macros for ESP32 (allows remapping in sketch or build flags) */
#if defined(ARDUINO_ARCH_ESP32)
#  ifndef RS485_UART_TX_PIN
#    define RS485_UART_TX_PIN  PIN_UART2_TX
#  endif
#  ifndef RS485_UART_RX_PIN
#    define RS485_UART_RX_PIN  PIN_UART2_RX
#  endif
#endif

// RS485_PORT defaults to Serial1 if not overridden by build flags or prior definitions.
