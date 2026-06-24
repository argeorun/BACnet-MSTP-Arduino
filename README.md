# BACnet MS/TP Arduino

A **BACnet/MSTP (RS-485) server device** running on Arduino-compatible hardware.  
Primarily targets the Arduino UNO (ATmega328P, 2 KB RAM / 32 KB Flash) as Milestone 1,
with a clear scaling roadmap for larger boards.

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
| Sketch entry point | `BACnet-Arduino-working.ino` | `setup()`, `loop()`, EEPROM bridge |
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

- **BACnet MS/TP** (RS-485, 9600–76800 baud, configurable MAC address)
- **12 Analog Value objects**: ADC channels 0–3 (AV:0–3), config registers
  (AV:92–95, writable), status (AV:96–99)
- **6 Binary Value objects**: digital I/O D3–D7 + LED output (BV:99, writable)
- **ReadProperty** and **WriteProperty** service support
- **Who-Is / I-Am** discovery support
- **EEPROM persistence**: device instance, MAC address, baud rate
- **Runtime self-diagnostics**: uptime (AV:99 hours), C-stack free byt ✅
- **Analog Value objects** (AV): ADC inputs (0–3), device config (92–95, writable), status (96–99) ✅
- **Binary Value objects** (BV): Digital I/O control & **LED output** (BV:99, writable, **TESTED** ✅)
- **ReadProperty** and **WriteProperty** service support (**TESTED** ✅)
- **Who-Is / I-Am** discovery support (**TESTED** ✅)
- **EEPROM persistence**: device instance, MAC address, baud rate ✅
- **Runtime self-diagnostics**: uptime (AV:99 hours), C-stack free bytes (AV:98) ✅
- Verified working with **YABE BACnet Explorer** over RS-485 (**LED CONTROL VERIFIED** ✅)
| RS-485 TX | D1 (TX) | UART to RS-485 transceiver DI |
| RS-485 RX | D0 (RX) | RS-485 transceiver RO → Arduino |
| RS-485 DE/RE | D2 | Direction control (HIGH = transmit) |
| Digital I/O | D3–D7 | BV:0–4 (BV:99 = onboard LED D13) |
| ADC inputs | A0–A3 | AV:0–3 |

> Any RS-485 transceiver compatible with 5 V logic works (e.g. MAX485, SN75176).

---

## Build & Upload

1. Open the workspace in **VS Code** with the Arduino extension installed.
2. Open `BACnet-Arduino-working.ino`.
3. **Verify**: `Ctrl+Alt+R`  
4. **Upload**: `Ctrl+Alt+U`  
   (Board: `arduino:avr:uno`, Port: your COM port)

### Current Build Stats (Milestone 1)

| Resource | Used | Total | % |
|----------|------|-------|---|
| Flash | 26,500 B | 32,256 B | 82% |
| RAM | 1,460 B | 2,048 B | 71% |

---

## Project Structure

```
BACnet-Arduino-working/
├── README.md                            ← this file
├── BACnet-Arduino-working.ino           ← sketch entry point
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
See `THIRD_PARTY_NOTICES.md` for scope and attribution mapping.
Local GPL text copy: `LICENSES/GPL-2.0-or-later.txt`
Upstream source: https://github.com/bacnet-stack/bacnet-stack

### Arduino port and platform abstraction layer (this project)
New files contributed in this repository — including `BACnet-Arduino-working.ino`,
`src/platform/rs485.cpp`, `src/platform/adc.c`, `src/platform/nvdata.c`,
`src/platform/pin_config.c`, `src/platform/stack.c`, `src/platform/timer.c`,
and `src/compile_config.h` — are released under the **MIT License**.

Copyright (c) 2025-2026 George Arun.

See [`LICENSE`](LICENSE) for the full MIT text that applies to these new files.

For files that contain upstream code plus local modifications, both upstream and local
attribution notices are intentionally preserved.
