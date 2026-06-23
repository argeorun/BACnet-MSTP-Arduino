---
applyTo: "**/*.c,**/*.h,**/*.cpp,**/*.ino"
---

# BACnet MS/TP Arduino Project — Reference Documentation

## Project Overview

This is a **BACnet MS/TP server device** running on Arduino hardware (primary target: ATmega328P / Uno, 2 KB RAM, 32 KB Flash). It is written in **pure C and C++ with no classes** — C++ is used only for Arduino library access in `rs485.cpp` and the `.ino` sketch entry point.

The stack is the open-source [bacnet-stack](https://github.com/bacnet-stack/bacnet-stack) library, stored under `src/bacnet/`. Root-level `.c` files are Arduino-customized overrides; the corresponding `src/bacnet/` originals are renamed `.disabled` to prevent linker duplicates.

---

## Directory Structure

```
BACnet-Arduino-working/
├── BACnet-Arduino-working.ino   # Arduino sketch: setup(), loop(), eeprom_bytes_*()
├── compile_config.h             # Centralized build flags (replaces Makefile -D flags)
└── src/
    ├── platform/                # Hardware abstraction layer (Arduino-specific)
    │   ├── adc.c / adc.h        # ADC abstraction (analogRead wrapper)
    │   ├── dlmstp.c             # MS/TP datalink (Arduino override)
    │   ├── nvdata.c / nvdata.h  # EEPROM non-volatile data abstraction
    │   ├── pin_config.c / pin_config.h  # Board-specific pin definitions
    │   ├── rs485.cpp / rs485.h  # RS-485 port abstraction (HardwareSerial)
    │   ├── rs485_port_config.h  # RS485_PORT selection (default: Serial)
    │   ├── stack.c / stack.h    # C stack usage monitoring
    │   └── timer.c              # millis()-based timer helpers
    ├── app/                     # BACnet application layer (objects + service handlers)
    │   ├── av.c                 # Analog Value BACnet object (Arduino override)
    │   ├── bv.c                 # Binary Value BACnet object (Arduino override)
    │   ├── device.c             # Device object (Arduino override)
    │   ├── apdu.c               # APDU handler (Arduino override)
    │   ├── h_rp.c               # ReadProperty handler (Arduino override)
    │   ├── h_wp.c               # WriteProperty handler (Arduino override)
    │   └── h_whois.c            # Who-Is handler (Arduino override)
    └── bacnet/                  # BACnet protocol stack library (upstream, untouched)
        ├── config.h             # Stack configuration (MAX_APDU, BACDL_MSTP, etc.)
        ├── bacdef.h             # Core BACnet types, macros, enums
        ├── bacenum.h            # All BACnet enumerations
        ├── bacdcode.h/.c        # Encode/decode primitives
        ├── bacstr.h/.c          # BACNET_CHARACTER_STRING
        ├── npdu.h/.c            # Network PDU layer
        ├── apdu.h               # APDU structures (confirmed/unconfirmed)
        ├── rp.h/.c              # ReadProperty encode/decode
        ├── wp.h/.c              # WriteProperty encode/decode
        ├── whois.h/.c           # Who-Is encode/decode
        ├── iam.h/.c             # I-Am encode/send
        ├── dcc.h/.c             # Device Communication Control
        ├── datalink/
        │   ├── datalink.h/.c    # Generic datalink API (abstraction)
        │   ├── dlmstp.h         # MS/TP API declarations
        │   ├── mstp.h/.c        # MS/TP state machine
        │   ├── mstpdef.h        # MS/TP constants and frame types
        │   ├── crc.h/.c         # MS/TP CRC8/CRC16
        │   └── (others disabled or not used)
        └── basic/
            ├── services.h       # Mega-include for ALL service handler headers
            ├── object/
            │   ├── device.h     # Device object API + object_*_function typedefs
            │   ├── av.h         # Analog Value object API + ANALOG_VALUE_DESCR
            │   └── bv.h         # Binary Value object API
            ├── service/
            │   ├── h_rp.h       # handler_read_property() declaration
            │   ├── h_wp.h       # handler_write_property() declaration
            │   └── h_whois.h    # handler_who_is() declaration
            ├── sys/
            │   ├── mstimer.h    # struct mstimer, mstimer_set/expired/reset
            │   ├── ringbuf.h    # Ring buffer for MS/TP receive
            │   └── platform.h   # Portable type helpers
            └── tsm/
                └── tsm.h        # Transaction State Machine (MAX_TSM_TRANSACTIONS=0, server-only)
```

---

## Include Path Rules

### `.ino` sketch (root) and `compile_config.h`
Stack headers use `src/bacnet/` prefix; platform headers use `src/platform/` prefix:
```c
#include "compile_config.h"              // stays at root
#include "src/platform/adc.h"
#include "src/platform/nvdata.h"
#include "src/platform/rs485.h"
#include "src/platform/stack.h"
#include "src/bacnet/datalink/dlmstp.h"
#include "src/bacnet/basic/services.h"
```

### `src/platform/` files
Stack headers use `../bacnet/` prefix; peer platform headers use bare names (same folder):
```c
#include "../bacnet/bacdef.h"
#include "../bacnet/datalink/dlmstp.h"
#include "../bacnet/basic/sys/mstimer.h"
#include "rs485.h"          // peer in same folder
#include "nvdata.h"         // peer in same folder
#include "pin_config.h"     // peer in same folder
```

### `src/app/` files
Stack headers use `../bacnet/` prefix; platform headers use `../platform/` prefix;
`compile_config.h` uses `../../compile_config.h` (two levels up to workspace root):
```c
#include "../../compile_config.h"        // device.c, apdu.c only
#include "../bacnet/bacdef.h"
#include "../bacnet/basic/object/av.h"
#include "../platform/adc.h"
#include "../platform/nvdata.h"
#include "../platform/rs485.h"
#include "../platform/pin_config.h"
```

### Files Inside `src/bacnet/` (stack library files — do not modify)
Use relative paths (NOT `bacnet/` prefix):
```c
// From src/bacnet/basic/object/av.h (depth 3):
#include "../../../bacnet/bacdef.h"
// From src/bacnet/datalink/dlmstp.h (depth 2):
#include "../bacdef.h"
#include "../basic/sys/ringbuf.h"
```
Note: The stack library headers have commented-out `bacnet/` style includes for reference.

---

## Compile-Time Configuration (`compile_config.h`)

Must be included BEFORE any BACnet headers. Replaces Makefile `-D` flags.

| Define | Value | Purpose |
|--------|-------|---------|
| `BACDL_MSTP` | 1 | MS/TP datalink (RS-485) |
| `WRITE_PROPERTY` | 1 | Enable WriteProperty service |
| `BACNET_SVC_SERVER` | 1 | Server mode |
| `BACAPP_REAL` | 1 | float support |
| `BACAPP_OBJECT_ID` | 1 | Object ID support |
| `BACAPP_UNSIGNED` | 1 | Unsigned int support |
| `BACAPP_ENUMERATED` | 1 | Enum (Binary Value) support |
| `BACAPP_CHARACTER_STRING` | 1 | String support |
| `BACNET_PROTOCOL_REVISION` | 9 | BACnet 2004 (BACnet-135-2008) |
| `BACNET_USE_DOUBLE` | 0 | Use `float` (4 B) not `double` (8 B) |
| `MAX_APDU` | 128 | APDU buffer size (RAM-constrained) |
| `MAX_TSM_TRANSACTIONS` | 0 | 0 = server-only mode, TSM disabled (saves RAM) |

---

## Key Data Structures

### `DLMSTP_PACKET` — `src/bacnet/datalink/dlmstp.h`
```c
typedef struct dlmstp_packet {
    bool ready;
    BACNET_ADDRESS address;
    uint8_t frame_type;
    uint16_t pdu_len;
    uint8_t pdu[DLMSTP_MPDU_MAX];  // DLMSTP_HEADER_MAX + MAX_PDU
} DLMSTP_PACKET;
```

### `DLMSTP_STATISTICS` — `src/bacnet/datalink/dlmstp.h`
```c
typedef struct dlmstp_statistics {
    uint32_t transmit_frame_counter;
    uint32_t receive_valid_frame_counter;
    uint32_t receive_invalid_frame_counter;
    uint32_t receive_valid_frame_not_for_us_counter;
    uint32_t transmit_pdu_counter;
    uint32_t receive_pdu_counter;
    uint32_t lost_token_counter;
    uint32_t bad_crc_counter;
    uint32_t poll_for_master_counter;
} DLMSTP_STATISTICS;
```

### `struct dlmstp_rs485_driver` — `src/bacnet/datalink/dlmstp.h`
RS-485 driver function pointer table used by the MS/TP state machine:
```c
struct dlmstp_rs485_driver {
    void (*init)(void);
    void (*send)(const uint8_t *payload, uint16_t payload_len);
    bool (*read)(uint8_t *buf);
    bool (*transmitting)(void);
    uint32_t (*baud_rate)(void);
    bool (*baud_rate_set)(uint32_t baud);
    uint32_t (*silence_milliseconds)(void);
    void (*silence_reset)(void);
};
```

### `struct mstp_flag_t` — `dlmstp.c` (internal, file-scope)
```c
static struct mstp_flag_t {
    unsigned ReceivedInvalidFrame  : 1;
    unsigned ReceivedValidFrame    : 1;
    unsigned SoleMaster            : 1;
    unsigned TransmitPacketPending : 1;
    unsigned TransmitPacketDER     : 1;
    unsigned ReceivePacketPending  : 1;
} MSTP_Flag;
```

### `ANALOG_VALUE_DESCR` — `src/bacnet/basic/object/av.h`
```c
typedef struct analog_value_descr {
    unsigned Event_State : 3;
    bool Out_Of_Service;
    BACNET_ENGINEERING_UNITS Units;
    float Present_Value;
    float Prior_Value;
    float COV_Increment;
    bool Changed;
    const char *Object_Name;
    const char *Description;
    BACNET_RELIABILITY Reliability;
    void *Context;
    // + INTRINSIC_REPORTING fields if enabled
} ANALOG_VALUE_DESCR;
```

### `struct object_data` (Analog Value) — `av.c` (file-scope)
```c
typedef float (*object_present_value_read_callback)(void);
typedef bool  (*object_present_value_write_callback)(float value);

struct object_data {
    const uint8_t object_id;
    const char *object_name;
    BACNET_ENGINEERING_UNITS units;
    object_present_value_read_callback read_callback;
    object_present_value_write_callback write_callback;
    float present_value;
};
```
Instances (Object_List in av.c): ADC0–ADC3 (mV), MCU freq, stack size, stack unused, MS/TP baud, MS/TP MAC, MS/TP max master, Device ID.

### `struct object_data` (Binary Value) — `bv.c` (file-scope)
```c
struct object_data {
    const uint8_t object_id;
    const char *object_name;
    uint8_t pin;
    bool is_output;
};
```
Instances: `{0,"D3",PIN_D3,true}`, `{1,"D4",...}`, `{2,"D5",...}`, `{3,"D6",...}`, `{4,"D7",...}`, `{99,"LED",PIN_LED,true}`.

### `struct mstimer` — `src/bacnet/basic/sys/mstimer.h`
General-purpose millisecond countdown timer using `millis()`.

### `BACNET_ADDRESS` — `src/bacnet/bacdef.h`
BACnet network address structure (MAC + net).

### `BACNET_READ_PROPERTY_DATA` — `src/bacnet/rp.h`
Request/response data for ReadProperty service.

### `BACNET_WRITE_PROPERTY_DATA` — `src/bacnet/wp.h`
Request/response data for WriteProperty service.

### `BACNET_CONFIRMED_SERVICE_DATA` — `src/bacnet/apdu.h`
Confirmed service request metadata (invoke ID, segmentation, priority).

---

## Function Reference by Module

### `adc.h` / `adc.c`
```c
void     adc_init(void);
void     adc_enable(uint8_t index);
uint8_t  adc_result_8bit(uint8_t index);
uint16_t adc_result_10bit(uint8_t index);
uint16_t adc_millivolts(uint8_t index);   // Used as AV read callback
```

### `rs485.h` / `rs485.cpp`
```c
void          RS485_Initialize(void);
void          RS485_Transmitter_Enable(bool enable);
void          RS485_Send_Data(const uint8_t *buffer, uint16_t nbytes);
bool          RS485_ReceiveError(void);
bool          RS485_DataAvailable(uint8_t *data);
void          RS485_Turnaround_Delay(void);
uint32_t      RS485_Get_Baud_Rate(void);
bool          RS485_Set_Baud_Rate(uint32_t baud);
uint32_t      RS485_Baud_Rate_From_Kilo(uint8_t baud_k);  // e.g. 38 → 38400
void          RS485_LED_Timers(void);
unsigned long RS485_Timer_Silence(void);
void          RS485_Timer_Silence_Reset(void);
```

### `nvdata.h` / `nvdata.c`
EEPROM address constants: `NV_EEPROM_TYPE_0=0`, `NV_EEPROM_MSTP_MAC=10`, `NV_EEPROM_MSTP_BAUD_K=11`, `NV_EEPROM_MSTP_MAX_MASTER=12`, `NV_EEPROM_DEVICE_0=13..15`.
```c
uint8_t  nvdata_unsigned8(uint32_t ee_address);
int      nvdata_unsigned8_set(uint32_t ee_address, uint8_t byte_value);
uint16_t nvdata_unsigned16(uint32_t ee_address);
int      nvdata_unsigned16_set(uint32_t ee_address, uint16_t short_value);
uint32_t nvdata_unsigned24(uint32_t ee_address);
int      nvdata_unsigned24_set(uint32_t ee_address, uint32_t long_value);
uint32_t nvdata_unsigned32(uint32_t ee_address);
int      nvdata_unsigned32_set(uint32_t ee_address, uint32_t long_value);
uint64_t nvdata_unsigned64(uint32_t ee_address);
int      nvdata_unsigned64_set(uint32_t ee_address, uint64_t long_value);
void     nvdata_name(uint32_t offset, BACNET_CHARACTER_STRING *char_string, const char *default_string);
bool     nvdata_name_write(uint32_t offset, BACNET_CHARACTER_STRING *char_string,
                           BACNET_ERROR_CLASS *error_class, BACNET_ERROR_CODE *error_code);
bool     nvdata_name_isvalid(uint8_t encoding, uint8_t length, const char *str);
bool     nvdata_name_set(uint16_t offset, uint8_t encoding, const char *str, uint8_t length);
uint32_t nvdata_get(uint32_t ee_address, uint8_t *buffer, uint32_t nbytes);
uint32_t nvdata_set(uint32_t ee_address, uint8_t *buffer, uint32_t nbytes);
```
Implemented via `eeprom_bytes_read()` / `eeprom_bytes_write()` defined in the `.ino` file (extern "C").

### `stack.h` / `stack.c`
```c
void     stack_init(void);
unsigned stack_size(void);
uint8_t  stack_byte(unsigned offset);
unsigned stack_unused(void);  // Used as AV read callback
```

### `pin_config.h` / `pin_config.c`
```c
void PIN_INIT(uint8_t pin, bool is_output);   // Calls pinMode()
void PIN_WRITE(uint8_t pin, bool active);      // Calls digitalWrite()
bool PIN_READ(uint8_t pin);                    // Calls digitalRead()
```
Board auto-detection macros: `PIN_D3..D8`, `PIN_LED`, `PIN_A0..A5`, `PIN_I2C_SDA/SCL`, `PIN_SPI_*`, `PIN_UART0_TX/RX`.

### `dlmstp.c` (root override) / `src/bacnet/datalink/dlmstp.h`
```c
void     dlmstp_init(char *ifname);              // Pass NULL for Arduino (uses RS485 module)
void     dlmstp_set_mac_address(uint8_t mac);
void     dlmstp_set_max_master(uint8_t max_master);
void     dlmstp_set_max_info_frames(uint8_t max_info_frames);
uint16_t dlmstp_receive(BACNET_ADDRESS *src, uint8_t *pdu,
                        uint16_t max_pdu, unsigned timeout);
int      dlmstp_send_pdu(BACNET_ADDRESS *dest, BACNET_NPDU_DATA *npdu_data,
                         uint8_t *pdu, uint16_t pdu_len);
void     dlmstp_get_my_address(BACNET_ADDRESS *my_address);
```
Internal state: `MSTP_RECEIVE_STATE`, `MSTP_MASTER_STATE`, `MSTP_Flag`, `This_Station`.

### `device.c` / `src/bacnet/basic/object/device.h`
```c
uint32_t Device_Object_Instance_Number(void);
bool     Device_Set_Object_Instance_Number(uint32_t object_id);
bool     Device_Valid_Object_Instance_Number(uint32_t object_id);
uint16_t Device_Vendor_Identifier(void);
unsigned Device_Object_List_Count(void);
bool     Device_Object_List_Identifier(uint32_t array_index,
                                        BACNET_OBJECT_TYPE *object_type,
                                        uint32_t *instance);
int      Device_Read_Property(BACNET_READ_PROPERTY_DATA *rpdata);
bool     Device_Write_Property(BACNET_WRITE_PROPERTY_DATA *wp_data);
```
Default: `Object_Instance_Number = 260001`, overridden from EEPROM `NV_EEPROM_DEVICE_0` → `260123`.

### `av.c` / `src/bacnet/basic/object/av.h`
```c
void     Analog_Value_Init(void);
bool     Analog_Value_Valid_Instance(uint32_t object_instance);
unsigned Analog_Value_Count(void);
uint32_t Analog_Value_Index_To_Instance(unsigned index);
unsigned Analog_Value_Instance_To_Index(uint32_t object_instance);
const char *Analog_Value_Name_ASCII(uint32_t object_instance);
bool     Analog_Value_Object_Name(uint32_t object_instance, BACNET_CHARACTER_STRING *object_name);
float    Analog_Value_Present_Value(uint32_t object_instance);
bool     Analog_Value_Present_Value_Set(uint32_t object_instance, float value, uint8_t priority);
int      Analog_Value_Read_Property(BACNET_READ_PROPERTY_DATA *rpdata);
bool     Analog_Value_Write_Property(BACNET_WRITE_PROPERTY_DATA *wp_data);
void     Analog_Value_Property_Lists(const int32_t **pRequired,
                                      const int32_t **pOptional,
                                      const int32_t **pProprietary);
```
`Analog_Value_Present_Value_Set(AV_UPTIME_INSTANCE=99, hours, priority=8)` called each second from `one_second_task()`.

### `bv.c` / `src/bacnet/basic/object/bv.h`
```c
void     Binary_Value_Init(void);
bool     Binary_Value_Valid_Instance(uint32_t object_instance);
unsigned Binary_Value_Count(void);
uint32_t Binary_Value_Index_To_Instance(unsigned index);
unsigned Binary_Value_Instance_To_Index(uint32_t object_instance);
const char *Binary_Value_Name_ASCII(uint32_t object_instance);
bool     Binary_Value_Object_Name(uint32_t object_instance, BACNET_CHARACTER_STRING *object_name);
BACNET_BINARY_PV Binary_Value_Present_Value(uint32_t instance);
bool     Binary_Value_Present_Value_Set(uint32_t object_instance,
                                         BACNET_BINARY_PV value, uint8_t priority);
int      Binary_Value_Read_Property(BACNET_READ_PROPERTY_DATA *rpdata);
bool     Binary_Value_Write_Property(BACNET_WRITE_PROPERTY_DATA *wp_data);
```
Writing to a BV with `is_output=true` calls `PIN_WRITE(obj->pin, active)`.

### `apdu.c` / `src/bacnet/apdu.h`
```c
uint8_t  apdu_network_priority(void);
void     apdu_network_priority_set(uint8_t pri);
bool     apdu_service_supported(BACNET_SERVICES_SUPPORTED service_supported);
uint16_t apdu_decode_confirmed_service_request(uint8_t *apdu, uint16_t apdu_len,
             BACNET_CONFIRMED_SERVICE_DATA *service_data,
             uint8_t *service_choice,
             uint8_t **service_request, uint16_t *service_request_len);
```

### `h_rp.c` — ReadProperty Handler
```c
void handler_read_property(uint8_t *service_request, uint16_t service_len,
                            BACNET_ADDRESS *src,
                            BACNET_CONFIRMED_SERVICE_DATA *service_data);
```
Dispatches to `Device_Read_Property()`, `Analog_Value_Read_Property()`, or `Binary_Value_Read_Property()` based on `rpdata.object_type`.

### `h_wp.c` — WriteProperty Handler
```c
void handler_write_property(uint8_t *service_request, uint16_t service_len,
                              BACNET_ADDRESS *src,
                              BACNET_CONFIRMED_SERVICE_DATA *service_data);
```
Dispatches to `Device_Write_Property()`, `Analog_Value_Write_Property()`, or `Binary_Value_Write_Property()`.

### `h_whois.c` — Who-Is Handler
```c
extern bool Send_I_Am_Flag;   // Global, checked in loop(), set here
void handler_who_is(uint8_t *service_request, uint16_t service_len,
                    BACNET_ADDRESS *src);
```

### `BACnet-Arduino-working.ino` — Main Sketch
```c
// EEPROM bridge (extern "C" linkage):
int eeprom_bytes_read(uint16_t eeaddr, uint8_t *buf, int len);
int eeprom_bytes_write(uint16_t eeaddr, const uint8_t *buf, int len);

// Arduino entry points:
void setup(void);   // hardware_init → Analog_Value_Init → Binary_Value_Init
                    // → device_nvdata_init → dlmstp_init(NULL) → mstimer_set
void loop(void);    // mstimer expired → one_second_task()
                    // dlmstp_receive() → npdu_handler()
```
`PDUBuffer[MAX_MPDU + 16]` — static receive buffer (144 bytes total).

---

## EEPROM Layout

| Address | Constant | Content |
|---------|----------|---------|
| 0–1 | `NV_EEPROM_TYPE_0/1` | Magic `0xBAC0` (init check) |
| 2 | `NV_EEPROM_VERSION` | Schema version `1` |
| 10 | `NV_EEPROM_MSTP_MAC` | MS/TP MAC address (default 10) |
| 11 | `NV_EEPROM_MSTP_BAUD_K` | Baud rate in kbps (default 38 = 38400) |
| 12 | `NV_EEPROM_MSTP_MAX_MASTER` | Max master (default 127) |
| 13–15 | `NV_EEPROM_DEVICE_0/1/2` | Device instance as 3 bytes (default 260123) |
| 32+ | `NV_EEPROM_DEVICE_NAME` | Device name string (30 bytes + encoding + length) |

---

## BACnet Objects Summary

| Type | Instance | Name | Source |
|------|----------|------|--------|
| Device | 260123 | (from EEPROM) | `device.c` |
| Analog Value | 0 | ADC0 mV | `av.c` / `adc_millivolts(0)` |
| Analog Value | 1 | ADC1 mV | `av.c` / `adc_millivolts(1)` |
| Analog Value | 2 | ADC2 mV | `av.c` / `adc_millivolts(2)` |
| Analog Value | 3 | ADC3 mV | `av.c` / `adc_millivolts(3)` |
| Analog Value | 92 | Device ID | `av.c` / `device_id()` → `nvdata_unsigned24(NV_EEPROM_DEVICE_0)` (writable) |
| Binary Value | 0 | D3 | `bv.c` / `PIN_D3` output |
| Binary Value | 1 | D4 | `bv.c` / `PIN_D4` output |
| Binary Value | 2 | D5 | `bv.c` / `PIN_D5` output |
| Binary Value | 3 | D6 | `bv.c` / `PIN_D6` output |
| Binary Value | 4 | D7 | `bv.c` / `PIN_D7` output |
| Binary Value | 99 | LED | `bv.c` / `PIN_LED=13` output |

---

## BACnet Services

| Service | Enabled | Handler |
|---------|---------|---------|
| ReadProperty | Always | `handler_read_property()` in `h_rp.c` |
| WriteProperty | `#ifdef WRITE_PROPERTY` | `handler_write_property()` in `h_wp.c` |
| Who-Is / I-Am | Always | `handler_who_is()` in `h_whois.c`, I-Am via `Send_I_Am_Flag` |

---

## Disabled / Overridden Files

Root-level Arduino overrides replace these `src/bacnet/` originals:

| Disabled | Root Override |
|----------|--------------|
| `src/bacnet/basic/object/device.c.disabled` | `device.c` |
| `src/bacnet/basic/object/av.c.disabled` | `av.c` |
| `src/bacnet/basic/object/bv.c.disabled` | `bv.c` |
| `src/bacnet/basic/service/h_rp.c.disabled` | `h_rp.c` |
| `src/bacnet/basic/service/h_wp.c.disabled` | `h_wp.c` |
| `src/bacnet/basic/service/h_whois.c.disabled` | `h_whois.c` |
| `src/bacnet/basic/service/h_apdu.c.disabled` | `apdu.c` |
| `src/bacnet/datalink/dlmstp.c.disabled` | `dlmstp.c` |
| `src/bacnet/basic/object/schedule.c.disabled` | RAM too large |
| `src/bacnet/basic/object/trendlog.c.disabled` | RAM too large |

---

## Memory Constraints (Arduino Uno)

- **RAM**: 2 KB total — currently near 100% utilization
- **Flash**: 32 KB — ~65% used
- `MAX_APDU = 128` (BACnet max is 1476)
- `MAX_TSM_TRANSACTIONS = 0` (server-only — device only responds, never initiates; stubs out TSM in `tsm.h`)
- Use `float` not `double` (`BACNET_USE_DOUBLE 0`)
- `PDUBuffer` is 144 bytes (128 + 16 header)
- See `MEMORY_OPTIMIZATION_PLAN.md` for scaling strategy

---

## C/C++ Linkage Rules

All C functions called from C code must have `extern "C"` guards in headers:
```c
#ifdef __cplusplus
extern "C" {
#endif
// ... declarations
#ifdef __cplusplus
}
#endif
```
The `eeprom_bytes_read()` / `eeprom_bytes_write()` functions are defined with `extern "C"` in the `.ino` file so C code (`nvdata.c`) can link them without name mangling.
