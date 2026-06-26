# BACnet MSTP Project for arduino boards

## 🎯 Current Development Status

**Latest Branch**: `master` (`r1-release`)  
**Status**: ✅ All boards hardware-validated (June 2026)

- ✅ LED control (BV:99) tested and working via YABE
- ✅ WriteProperty implemented and tested
- ✅ Error handling verified (invalid values correctly rejected)
- ✅ Full feature set: ReadProperty, WriteProperty, Who-Is/I-Am discovery
- ✅ Multi-board support: Uno, Mega 2560, ESP32, ESP32-S3, STM32 F103C8, STM32 F756ZG
- ✅ All 5 boards confirmed working with YABE after v0.0.3 RS485 port fix (June 26, 2026)

**Previous Branch**: `Minimal-BACnet-mstp` (LED control milestone — preserved)

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
| Sketch entry point | `BACnet-MSTP-Arduino.ino` | `setup()`, `loop()`, EEPROM bridge |
| Platform abstraction | `src/platform/` | Arduino-specific HAL |
| RS-485 port | `src/platform/rs485.cpp` | HardwareSerial + DE/RE pin control |
| ADC abstraction | `src/platform/adc.c` | `analogRead()` wrapper |
| EEPROM (non-volatile) | `src/platform/nvdata.c` | EEPROM-backed MAC, baud, device ID |
| Pin configuration | `src/platform/pin_config.c` | Board pin assignments |
| C stack monitor | `src/platform/stack.c` | `stack_unused()` health metric |
| MS/TP datalink | `src/platform/dlmstp.c` | Arduino-adapted MS/TP datalink |
| BACnet app objects | `src/app/` | Device, AV ×12, BV ×6 objects |
| Build configuration | `src/compile_config.h` | Centralised stack parameter overrides |

---

## Features

- **BACnet MS/TP** (RS-485, 9600–1,152,000 baud, configurable MAC address) ✅
- **Analog Value objects** (AV ×12): ADC inputs (AV:0–3), device config (AV:92–95, writable), status (AV:96–99) ✅
- **Binary Value objects** (BV ×6): Digital outputs D3–D7 (BV:0–4) & **LED** (BV:99, **TESTED** ✅)
- **ReadProperty** and **WriteProperty** service support (**TESTED** ✅)
- **Who-Is / I-Am** discovery support (**TESTED** ✅)
- **EEPROM persistence**: device instance, MAC address, baud rate ✅
- **Runtime self-diagnostics**: uptime (AV:99 hours), C-stack free bytes (AV:98) ✅
- Verified working with **YABE BACnet Explorer** over RS-485 (**LED CONTROL VERIFIED** ✅)

---

## Hardware Support

### Arduino Uno (AVR, 5 V)

| Signal | Pin | Notes |
|--------|-----|-------|
| RS-485 TX | D1 (TX) | UART to RS-485 transceiver DI |
| RS-485 RX | D0 (RX) | RS-485 transceiver RO → Arduino |
| RS-485 DE/RE | D2 | Direction control (HIGH = transmit) |
| Digital I/O | D3–D7 | BV:0–4 (BV:99 = LED D13) |
| ADC inputs | A0–A3 | AV:0–3 |

> **Note:** Uno uses its only UART for RS-485. USB serial debug is not available while running.

### Arduino Mega 2560

| Signal | Pin | Notes |
|--------|-----|-------|
| RS-485 TX | TX1 (D18) | Serial1 to transceiver DI |
| RS-485 RX | RX1 (D19) | Transceiver RO → Mega |
| RS-485 DE/RE | D8 (PIN_D8) | Direction control |
| Digital I/O | D3–D7 | BV:0–4 (BV:99 = LED D13) |
| ADC inputs | A0–A3 | AV:0–3 |

### STM32 F103C8 (Blue Pill / Black Pill)

| Signal | Pin | Notes |
|--------|-----|-------|
| RS-485 TX | PB10 (USART3) | Declared as `Serial_RS485` in library |
| RS-485 RX | PB11 (USART3) | |
| RS-485 DE/RE | PA12 (PIN_D8) | |
| Digital I/O | PB3–PB5, PA10–PA11 | BV:0–4 (BV:99 = LED PB7) |
| ADC inputs | PA3, PC0, PC3, PF3 | AV:0–3 |

### STM32 Nucleo-144 (F756ZG / F746ZG / F429ZI)

| Signal | Pin | Notes |
|--------|-----|-------|
| RS-485 TX | PG14 (USART6) | Declared as `Serial6` in library |
| RS-485 RX | PG9 (USART6) | |
| RS-485 DE/RE | PA12 | Change to any free GPIO |

### ESP32 (generic)

| Signal | Pin | Notes |
|--------|-----|-------|
| RS-485 TX | GPIO17 (Serial2) | |
| RS-485 RX | GPIO16 (Serial2) | |
| RS-485 DE/RE | GPIO18 (PIN_D8) | |
| Digital I/O | GPIO4,5,19,23,27 | BV:0–4 (BV:99 = LED GPIO2) |
| ADC inputs | GPIO32–35 | AV:0–3 |

### ESP32-S3

| Signal | Pin | Notes |
|--------|-----|-------|
| RS-485 TX | GPIO17 (Serial2) | |
| RS-485 RX | GPIO16 (Serial2) | |
| RS-485 DE/RE | GPIO2 (PIN_D8) | |
| Digital I/O | GPIO3,14,15,18,47 | BV:0–4 (BV:99 = LED RGB_BUILTIN) |
| ADC inputs | GPIO4–7 | AV:0–3 |

> Any RS-485 transceiver compatible with the board's logic level works (e.g. MAX485, SN75176, SP3485).

---

## 📥 Download & Installation Guide

### Option A: Clone from GitHub (Recommended)

```bash
# Clone the master branch (R1 release)
git clone -b master https://github.com/argeorun/BACnet-MSTP-Arduino.git BACnet-MSTP-Arduino

# Or clone the r1-release branch (identical)
git clone -b r1-release https://github.com/argeorun/BACnet-MSTP-Arduino.git BACnet-MSTP-Arduino

# Navigate to the project folder
cd BACnet-MSTP-Arduino

# Open BACnet-MSTP-Arduino.ino in Arduino IDE
```

### Option B: Download as ZIP

**Step 1: Download ZIP from GitHub**
- Go to: https://github.com/argeorun/BACnet-MSTP-Arduino
- Click **Code** → **Download ZIP**
- Choose branch: `master` (R1 release — recommended)

**Step 2: Extract the ZIP file**

After extraction, you'll have this structure:
```
BACnet-MSTP-Arduino-master/         ← GitHub wrapper (outer folder)
  └── BACnet-MSTP-Arduino.ino     ← sketch (open this folder in Arduino IDE)
  └── src/
  └── README.md
  └── ...
```

**⚠️ IMPORTANT:** Open the extracted folder directly in Arduino IDE — it contains `BACnet-MSTP-Arduino.ino`.

**Step 3: Open in Arduino IDE**
1. Move or extract the `BACnetMSTP-Arduino` folder to your Arduino projects directory:
   - **Windows**: `Documents\Arduino\` 
   - **Mac**: `~/Documents/Arduino/`
   - **Linux**: `~/Arduino/`

2. In Arduino IDE, click **File** → **Open**
4. Navigate to: `Arduino/BACnet-MSTP-Arduino/BACnet-MSTP-Arduino.ino`
4. Click **Open**

---

## ⚙️ Configure Arduino IDE

Before uploading, verify settings:

1. **Board**: Select `Arduino UNO`
   - Tools → Board → Arduino AVR Boards → Arduino UNO

2. **Port**: Select your USB COM port
   - Tools → Port → COM3 (or your device's port)
   - On Mac: `/dev/cu.usbserial-*`
   - On Linux: `/dev/ttyUSB0`

3. **Programmer**: Select `AVR ISP` (default)
   - Tools → Programmer → AVR ISP

---

## 🔨 Compile & Upload

Once configured:

**Option 1: Using Arduino IDE GUI**
- Click **Verify** (✓) to compile
- Click **Upload** (→) to program the board

**Option 2: Using VS Code with Arduino Extension**
- **Verify**: `Ctrl+Alt+R`
- **Upload**: `Ctrl+Alt+U`

**Result**: Board flashes and joins the MS/TP network. Debug output via Serial is available on Mega/ESP32/STM32 boards (Serial Monitor at 115200 baud). Not available on Uno (RS-485 uses the only UART).

---

## ✅ Verify Installation

After uploading successfully:

1. **Open Serial Monitor** (Tools → Serial Monitor, 9600 baud)
   - Should see device boot messages

2. **Test with YABE BACnet Explorer**
   - [Download YABE](https://sourceforge.net/projects/yetanotherbacnetexplorer/)
   - Device should appear on MS/TP network
   - Test LED control: Write BV:99 = 1 or 0

3. **Check LED Response**
   - Write BV:99 PRESENT_VALUE = 1 → LED turns ON
   - Write BV:99 PRESENT_VALUE = 0 → LED turns OFF
   - Expected response: < 100ms

> **Uno note:** Serial Monitor is not available while BACnet runs — the Uno's only UART is used for RS-485. Use a Mega or ESP32 to get simultaneous USB debug output.

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
BACnet-MSTP-Arduino/
├── README.md                            ← this file
├── BACnet-MSTP-Arduino.ino            ← sketch entry point
├── src/
│   ├── compile_config.h                 ← stack build flags
│   ├── platform/                        ← Arduino HAL (new code)
│   │   ├── rs485.cpp / rs485.h
│   │   ├── dlmstp.c                     ← forked + Arduino-adapted
│   │   ├── adc.c / adc.h
│   │   ├── nvdata.c / nvdata.h
│   │   ├── pin_config.c / pin_config.h
│   │   ├── stack.c / stack.h
│   │   └── timer.c
│   ├── app/                             ← BACnet objects/handlers
│   │   ├── device.c                     ← forked + adapted from bacnet-stack
│   │   ├── av.c                         ← forked + adapted from bacnet-stack
│   │   ├── bv.c                         ← forked + adapted from bacnet-stack
│   │   ├── apdu.c / h_rp.c / h_wp.c / h_whois.c
│   └── bacnet/                          ← upstream bacnet-stack (unmodified)
└── .vscode/
    ├── arduino.json
    └── c_cpp_properties.json
```

---

## Multi-Board Support

| Board | RAM | Compile | Upload & YABE tested |
|-------|-----|---------|----------------------|
| Arduino Uno | 2 KB | ✅ | ✅ (v0.0.3, June 26 2026) |
| Arduino Mega 2560 | 8 KB | ✅ | ✅ (v0.0.3, June 26 2026) |
| ESP32 (generic) | 520 KB | ✅ | ✅ (v0.0.3, June 26 2026) |
| ESP32-S3 | 512 KB | ✅ | ✅ (v0.0.3, June 26 2026) |
| STM32 F103C8 (Blue Pill) | 20 KB | ✅ | ✅ (v0.0.3, June 26 2026) |
| STM32 Nucleo-144 F756ZG | 320 KB | ✅ | ✅ (v0.0.3, June 26 2026) |
| Arduino Due | 96 KB | 🔲 Planned | 🔲 |

---

## License

This repository contains code under two licenses:

### Upstream bacnet-stack (Steve Karg and contributors)
Files under `src/bacnet/` and derived adaptations in `src/app/` and `src/platform/dlmstp.c`
are covered by their original upstream licenses:

- `GPL-2.0-or-later WITH GCC-exception-2.0` — majority of protocol stack files
- `MIT` — selected utility and object files (see per-file `@copyright` tag)

The **GCC Runtime Library Exception** allows the GPL-licensed stack to be linked
into firmware without the GPL copyleft extending to the application layer.  
See `THIRD_PARTY_NOTICES.md` for scope and attribution mapping.
Local GPL text copy: `LICENSES/GPL-2.0-or-later.txt`
Upstream source: https://github.com/bacnet-stack/bacnet-stack

### Arduino port and platform abstraction layer (this project)
New files contributed in this repository — including `BACnet-MSTP-Arduino.ino`,
`src/platform/rs485.cpp`, `src/platform/adc.c`, `src/platform/nvdata.c`,
`src/platform/pin_config.c`, `src/platform/stack.c`, `src/platform/timer.c`,
and `src/compile_config.h` — are released under the **MIT License**.

Copyright (c) 2025-2026 George Arun.

See [`LICENSE`](LICENSE) for the full MIT text that applies to these new files.
For files that contain upstream code plus local modifications, both upstream and local
attribution notices are intentionally preserved.
