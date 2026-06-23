# BACnet MSTP Project for arduino boards

## ðŸŽ¯ Current Development Status

**Latest Branch**: `master` (`r1-release`)  
**Status**: âœ… Multi-board verified (June 2026)

- âœ… LED control (BV:99) tested and working via YABE
- âœ… WriteProperty implemented and tested
- âœ… Error handling verified (invalid values correctly rejected)
- âœ… Full feature set: ReadProperty, WriteProperty, Who-Is/I-Am discovery
- âœ… Multi-board support: Uno, Mega 2560, ESP32, STM32 F103C8, STM32 F756ZG

**Previous Branch**: `Minimal-BACnet-mstp` (LED control milestone â€” preserved)

---

## Based On

This project is a port and adaptation of the
**[bacnet-stack](https://github.com/bacnet-stack/bacnet-stack)** open-source library
originally written by **Steve Karg** and contributors.

The upstream library provides the complete BACnet protocol stack (encoding/decoding,
MS/TP state machine, service handlers). This repository adapts the AVR port of that
library for the Arduino IDE build system and Arduino hardware peripherals.

See [`src/bacnet/`](src/bacnet/) for the upstream library files (unmodified, all credit
to the original authors). See [`LICENSE`](LICENSE) for the full upstream license notice.

---

## What This Project Adds

| Layer | Files | Description |
|-------|-------|-------------|
| Sketch entry point | `BACnetMSTP-Arduino.ino` | `setup()`, `loop()`, EEPROM bridge |
| Platform abstraction | `src/platform/` | Arduino-specific HAL |
| RS-485 port | `src/platform/rs485.cpp` | HardwareSerial + DE/RE pin control |
| ADC abstraction | `src/platform/adc.c` | `analogRead()` wrapper |
| EEPROM (non-volatile) | `src/platform/nvdata.c` | EEPROM-backed MAC, baud, device ID |
| Pin configuration | `src/platform/pin_config.c` | Board pin assignments |
| C stack monitor | `src/platform/stack.c` | `stack_unused()` health metric |
| MS/TP datalink | `src/platform/dlmstp.c` | Arduino-adapted MS/TP datalink |
| BACnet app objects | `src/app/` | Device, AV Ã—12, BV Ã—6 objects |
| Build configuration | `src/compile_config.h` | Centralised stack parameter overrides |

---

## Features

- **BACnet MS/TP** (RS-485, 9600â€“1,152,000 baud, configurable MAC address) âœ…
- **Analog Value objects** (AV Ã—12): ADC inputs (AV:0â€“3), device config (AV:92â€“95, writable), status (AV:96â€“99) âœ…
- **Binary Value objects** (BV Ã—6): Digital outputs D3â€“D7 (BV:0â€“4) & **LED** (BV:99, **TESTED** âœ…)
- **ReadProperty** and **WriteProperty** service support (**TESTED** âœ…)
- **Who-Is / I-Am** discovery support (**TESTED** âœ…)
- **EEPROM persistence**: device instance, MAC address, baud rate âœ…
- **Runtime self-diagnostics**: uptime (AV:99 hours), C-stack free bytes (AV:98) âœ…
- Verified working with **YABE BACnet Explorer** over RS-485 (**LED CONTROL VERIFIED** âœ…)

---

## Hardware Support

### Arduino Uno (AVR, 5 V)

| Signal | Pin | Notes |
|--------|-----|-------|
| RS-485 TX | D1 (TX) | UART to RS-485 transceiver DI |
| RS-485 RX | D0 (RX) | RS-485 transceiver RO â†’ Arduino |
| RS-485 DE/RE | D2 | Direction control (HIGH = transmit) |
| Digital I/O | D3â€“D7 | BV:0â€“4 (BV:99 = LED D13) |
| ADC inputs | A0â€“A3 | AV:0â€“3 |

> **Note:** Uno uses its only UART for RS-485. USB serial debug is not available while running.

### Arduino Mega 2560

| Signal | Pin | Notes |
|--------|-----|-------|
| RS-485 TX | TX1 (D18) | Serial1 to transceiver DI |
| RS-485 RX | RX1 (D19) | Transceiver RO â†’ Mega |
| RS-485 DE/RE | D8 (PIN_D8) | Direction control |
| Digital I/O | D3â€“D7 | BV:0â€“4 (BV:99 = LED D13) |
| ADC inputs | A0â€“A3 | AV:0â€“3 |

### STM32 F103C8 (Blue Pill / Black Pill)

| Signal | Pin | Notes |
|--------|-----|-------|
| RS-485 TX | PB10 (USART3) | Declared as `Serial_RS485` in library |
| RS-485 RX | PB11 (USART3) | |
| RS-485 DE/RE | PA12 (PIN_D8) | |
| Digital I/O | PB3â€“PB5, PA10â€“PA11 | BV:0â€“4 (BV:99 = LED PB7) |
| ADC inputs | PA3, PC0, PC3, PF3 | AV:0â€“3 |

### STM32 Nucleo-144 (F756ZG / F746ZG / F429ZI)

| Signal | Pin | Notes |
|--------|-----|-------|
| RS-485 TX | PG14 (USART6) | Declared as `Serial6` in library |
| RS-485 RX | PG9 (USART6) | |
| RS-485 DE/RE | PA12 | Change to any free GPIO |

### ESP32

| Signal | Pin | Notes |
|--------|-----|-------|
| RS-485 TX | GPIO25 (Serial1) | |
| RS-485 RX | GPIO26 (Serial1) | |
| RS-485 DE/RE | GPIO18 (PIN_D8) | |
| Digital I/O | GPIO4,5,19,23,27 | BV:0â€“4 (BV:99 = LED GPIO2) |
| ADC inputs | GPIO32â€“35 | AV:0â€“3 |

> Any RS-485 transceiver compatible with the board's logic level works (e.g. MAX485, SN75176, SP3485).

---

## ðŸ“¥ Download & Installation Guide

### Option A: Clone from GitHub (Recommended)

```bash
# Clone the master branch (R1 release)
git clone -b master https://github.com/argeorun/BACnet-MSTP-Arduino.git BACnetMSTP-Arduino-R1

# Or clone the r1-release branch (identical)
git clone -b r1-release https://github.com/argeorun/BACnet-MSTP-Arduino.git BACnetMSTP-Arduino-R1

# Navigate to the project folder
cd BACnetMSTP-Arduino-R1

# Open BACnetMSTP-Arduino.ino in Arduino IDE
```

### Option B: Download as ZIP

**Step 1: Download ZIP from GitHub**
- Go to: https://github.com/argeorun/BACnet-MSTP-Arduino
- Click **Code** â†’ **Download ZIP**
- Choose branch: `master` (R1 release â€” recommended)

**Step 2: Extract the ZIP file**

After extraction, you'll have this structure:
```
BACnet-MSTP-Arduino-master/         â† GitHub wrapper (outer folder)
  â””â”€â”€ BACnetMSTP-Arduino.ino     â† sketch (open this folder in Arduino IDE)
  â””â”€â”€ src/
  â””â”€â”€ README.md
  â””â”€â”€ ...
```

**âš ï¸ IMPORTANT:** Open the extracted folder directly in Arduino IDE â€” it contains `BACnetMSTP-Arduino.ino`.

**Step 3: Open in Arduino IDE**
1. Move or extract the `BACnetMSTP-Arduino` folder to your Arduino projects directory:
   - **Windows**: `Documents\Arduino\` 
   - **Mac**: `~/Documents/Arduino/`
   - **Linux**: `~/Arduino/`

2. In Arduino IDE, click **File** â†’ **Open**
4. Navigate to: `Arduino/BACnetMSTP-Arduino-R1/BACnetMSTP-Arduino.ino`
4. Click **Open**

---

## âš™ï¸ Configure Arduino IDE

Before uploading, verify settings:

1. **Board**: Select `Arduino UNO`
   - Tools â†’ Board â†’ Arduino AVR Boards â†’ Arduino UNO

2. **Port**: Select your USB COM port
   - Tools â†’ Port â†’ COM3 (or your device's port)
   - On Mac: `/dev/cu.usbserial-*`
   - On Linux: `/dev/ttyUSB0`

3. **Programmer**: Select `AVR ISP` (default)
   - Tools â†’ Programmer â†’ AVR ISP

---

## ðŸ”¨ Compile & Upload

Once configured:

**Option 1: Using Arduino IDE GUI**
- Click **Verify** (âœ“) to compile
- Click **Upload** (â†’) to program the board

**Option 2: Using VS Code with Arduino Extension**
- **Verify**: `Ctrl+Alt+R`
- **Upload**: `Ctrl+Alt+U`

**Result**: Board flashes and joins the MS/TP network. Debug output via Serial is available on Mega/ESP32/STM32 boards (Serial Monitor at 115200 baud). Not available on Uno (RS-485 uses the only UART).

---

## âœ… Verify Installation

After uploading successfully:

1. **Open Serial Monitor** (Tools â†’ Serial Monitor, 9600 baud)
   - Should see device boot messages

2. **Test with YABE BACnet Explorer**
   - [Download YABE](https://sourceforge.net/projects/yetanotherbacnetexplorer/)
   - Device should appear on MS/TP network
   - Test LED control: Write BV:99 = 1 or 0

3. **Check LED Response**
   - Write BV:99 PRESENT_VALUE = 1 â†’ LED turns ON
   - Write BV:99 PRESENT_VALUE = 0 â†’ LED turns OFF
   - Expected response: < 100ms

> **Uno note:** Serial Monitor is not available while BACnet runs â€” the Uno's only UART is used for RS-485. Use a Mega or ESP32 to get simultaneous USB debug output.

---

## Current Build Stats

| Board | Flash used | Flash total | Flash % | RAM used | RAM total | RAM % |
|-------|-----------|-------------|---------|---------|-----------|-------|
| Arduino Uno | 28,054 B | 32,256 B | 86% | 1,631 B | 2,048 B | 79% |
| STM32 F103C8 | 37,020 B | 65,536 B | 56% | 3,716 B | 20,480 B | 18% |
| ESP32 | 317,903 B | 1,310,720 B | 24% | 22,056 B | 327,680 B | 6% |
| STM32 F756ZG | 42,636 B | 1,048,576 B | 4% | 11,200 B | 327,680 B | 3% |

---

## Project Structure

```
BACnetMSTP-Arduino-R1/
â”œâ”€â”€ README.md                            â† this file
â”œâ”€â”€ BACnetMSTP-Arduino.ino            â† sketch entry point
â”œâ”€â”€ src/
â”‚   â”œâ”€â”€ compile_config.h                 â† stack build flags
â”‚   â”œâ”€â”€ platform/                        â† Arduino HAL (new code)
â”‚   â”‚   â”œâ”€â”€ rs485.cpp / rs485.h
â”‚   â”‚   â”œâ”€â”€ dlmstp.c                     â† forked + Arduino-adapted
â”‚   â”‚   â”œâ”€â”€ adc.c / adc.h
â”‚   â”‚   â”œâ”€â”€ nvdata.c / nvdata.h
â”‚   â”‚   â”œâ”€â”€ pin_config.c / pin_config.h
â”‚   â”‚   â”œâ”€â”€ stack.c / stack.h
â”‚   â”‚   â””â”€â”€ timer.c
â”‚   â”œâ”€â”€ app/                             â† BACnet objects/handlers
â”‚   â”‚   â”œâ”€â”€ device.c                     â† forked + adapted from bacnet-stack
â”‚   â”‚   â”œâ”€â”€ av.c                         â† forked + adapted from bacnet-stack
â”‚   â”‚   â”œâ”€â”€ bv.c                         â† forked + adapted from bacnet-stack
â”‚   â”‚   â”œâ”€â”€ apdu.c / h_rp.c / h_wp.c / h_whois.c
â”‚   â””â”€â”€ bacnet/                          â† upstream bacnet-stack (unmodified)
â””â”€â”€ .vscode/
    â”œâ”€â”€ arduino.json
    â””â”€â”€ c_cpp_properties.json
```

---

## Multi-Board Support

| Board | RAM | Compile | Upload tested |
|-------|-----|---------|---------------|
| Arduino Uno | 2 KB | âœ… | âœ… |
| Arduino Mega 2560 | 8 KB | âœ… | ðŸ”² |
| ESP32 | 520 KB | âœ… | ðŸ”² |
| STM32 F103C8 (Blue Pill) | 20 KB | âœ… | ðŸ”² |
| STM32 Nucleo-144 F756ZG | 320 KB | âœ… | ðŸ”² |
| Arduino Due | 96 KB | ðŸ”² Planned | ðŸ”² |
| ESP32-S3 | 512 KB | ðŸ”² Planned | ðŸ”² |

---

## License

This repository contains code under two licenses:

### Upstream bacnet-stack (Steve Karg and contributors)
Files under `src/bacnet/` and derived adaptations in `src/app/` and `src/platform/dlmstp.c`
are covered by their original upstream licenses:

- `GPL-2.0-or-later WITH GCC-exception-2.0` â€” majority of protocol stack files
- `MIT` â€” selected utility and object files (see per-file `@copyright` tag)

The **GCC Runtime Library Exception** allows the GPL-licensed stack to be linked
into firmware without the GPL copyleft extending to the application layer.  
Full upstream license: https://github.com/bacnet-stack/bacnet-stack

### Arduino port and platform abstraction layer (this project)
New files contributed in this repository â€” including `BACnetMSTP-Arduino.ino`,
`src/platform/rs485.cpp`, `src/platform/adc.c`, `src/platform/nvdata.c`,
`src/platform/pin_config.c`, `src/platform/stack.c`, `src/platform/timer.c`,
and `src/compile_config.h` â€” are released under the **MIT License**.

See [`LICENSE`](LICENSE) for the full MIT text that applies to these new files.

