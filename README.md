## 🎯 Current Development Status

**Latest Branch**: `Minimal-BACnet-mstp`  
**Status**: ✅ LED Control Verified (March 23, 2026)

- ✅ LED control (BV:99) tested and working via YABE
- ✅ WriteProperty implemented and tested
- ✅ Error handling verified (invalid values correctly rejected)
- ⏰ Full feature set: ReadProperty, WriteProperty, discovery
- 📦 Ready for Arduino Library Manager packaging

**Previous Release**: `master` (Initial commit: BACnet MS/TP Arduino port)

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
| BACnet app objects | `src/app/` | Device, AV ×12, BV ×6 objects |
| Build configuration | `src/compile_config.h` | Centralised stack parameter overrides |

---

## Features

- **BACnet MS/TP** (RS-485, 9600–76800 baud, configurable MAC address) ✅
- **Analog Value objects** (AV): ADC inputs (0–3), device config (92–95, writable), status (96–99) ✅
- **Binary Value objects** (BV): Digital I/O control & **LED output** (BV:99, writable, **TESTED** ✅)
- **ReadProperty** and **WriteProperty** service support (**TESTED** ✅)
- **Who-Is / I-Am** discovery support (**TESTED** ✅)
- **EEPROM persistence**: device instance, MAC address, baud rate ✅
- **Runtime self-diagnostics**: uptime (AV:99 hours), C-stack free bytes (AV:98) ✅
- Verified working with **YABE BACnet Explorer** over RS-485 (**LED CONTROL VERIFIED** ✅)

---

## Hardware — Milestone 1 (Arduino UNO)

| Signal | Arduino UNO Pin | Notes |
|--------|----------------|-------|
| RS-485 TX | D1 (TX) | UART to RS-485 transceiver DI |
| RS-485 RX | D0 (RX) | RS-485 transceiver RO → Arduino |
| RS-485 DE/RE | D2 | Direction control (HIGH = transmit) |
| Digital I/O | D3–D7 | BV:0–4 (BV:99 = onboard LED D13) |
| ADC inputs | A0–A3 | AV:0–3 |

> Any RS-485 transceiver compatible with 5 V logic works (e.g. MAX485, SN75176).

---

## 📥 Download & Installation Guide

### Option A: Clone from GitHub (Recommended)

```bash
# Clone the main testing branch
git clone -b Minimal-BACnet-mstp https://github.com/argeorun/BACnet-MSTP-Arduino.git BACnetMSTP-Arduino

# Navigate to the project folder
cd BACnetMSTP-Arduino

# Open in Arduino IDE
```

### Option B: Download as ZIP

**Step 1: Download ZIP from GitHub**
- Go to: https://github.com/argeorun/BACnet-MSTP-Arduino
- Click **Code** → **Download ZIP**
- Choose branch: `Minimal-BACnet-mstp` (LED control) or `master` (initial release)

**Step 2: Extract the ZIP file**

After extraction, you'll have this structure:
```
BACnet-MSTP-Arduino-Minimal-BACnet-mstp/    ← GitHub wrapper (outer folder)
  └── BACnetMSTP-Arduino/                   ← Project folder (open THIS one)
      ├── BACnetMSTP-Arduino.ino
      ├── src/
      ├── README.md
      └── ...
```

**⚠️ IMPORTANT:** Do NOT open the outer `BACnet-MSTP-Arduino-Minimal-BACnet-mstp` folder  
→ **Open the inner `BACnetMSTP-Arduino` folder** (it contains the `.ino` file)

**Step 3: Open in Arduino IDE**
1. Move or extract the `BACnetMSTP-Arduino` folder to your Arduino projects directory:
   - **Windows**: `Documents\Arduino\` 
   - **Mac**: `~/Documents/Arduino/`
   - **Linux**: `~/Arduino/`

2. In Arduino IDE, click **File** → **Open**
3. Navigate to: `Arduino/BACnetMSTP-Arduino/BACnetMSTP-Arduino.ino`
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

**Result**: Board should boot and print debug info to **Serial Monitor** (9600 baud)

---

## ✅ Verify Installation

After uploading successfully:

1. **Open Serial Monitor** (Tools → Serial Monitor, 9600 baud)
   - Should see device boot messages

2. **Test with YABE BACnet Explorer**
   - [Download YABE](https://sourceforge.net/projects/yetanotherbacnetexplorer/)
   - Device should appear on MS/TP network
   - Test LED control: Write BV:99 = 1 or 0

3. **Check LED Response** (Minimal-BACnet-mstp branch)
   - Write BV:99 PRESENT_VALUE = 1 → LED turns ON
   - Write BV:99 PRESENT_VALUE = 0 → LED turns OFF
   - Expected response: < 100ms

---

## Current Build Stats

| Resource | Used | Total | % | Status |
|----------|------|-------|---|--------|
| Flash | 26,500 B | 32,256 B | 82% | ✅ Within target |
| RAM | 1,460 B | 2,048 B | 71% | ✅ Within target |

---

## Project Structure

```
BACnetMSTP-Arduino/
├── README.md                            ← this file
├── BACnetMSTP-Arduino.ino               ← sketch entry point
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

## Multi-Board Roadmap

| Milestone | Board | RAM | Status |
|-----------|-------|-----|--------|
| M1 | Arduino UNO | 2 KB | ✅ Complete |
| M2 | Arduino Mega 2560 | 8 KB | 🔲 Planned |
| M3 | Arduino Due | 96 KB | 🔲 Planned |
| M4 | Arduino Zero / MKR | 32 KB | 🔲 Planned |
| M5 | ESP32 | 520 KB | 🔲 Planned |
| M6 | STM32F1 | 20 KB | 🔲 Planned |
| M7 | STM32F4 | 128 KB | 🔲 Planned |

More boards will be added in future milestones as the codebase scales.

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
Full upstream license: https://github.com/bacnet-stack/bacnet-stack

### Arduino port and platform abstraction layer (this project)
New files contributed in this repository — including `BACnetMSTP-Arduino.ino`,
`src/platform/rs485.cpp`, `src/platform/adc.c`, `src/platform/nvdata.c`,
`src/platform/pin_config.c`, `src/platform/stack.c`, `src/platform/timer.c`,
and `src/compile_config.h` — are released under the **MIT License**.

See [`LICENSE`](LICENSE) for the full MIT text that applies to these new files.
