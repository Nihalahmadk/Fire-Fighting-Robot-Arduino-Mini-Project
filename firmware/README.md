# Firmware

## Status

| Item | Status |
|---|---|
| Original 2023–24 sketch | **Not available** (not in this repository) |
| `FireFightingRobot/` | **New reference implementation**, written in 2026 from the report's design. Compiles for the Arduino Uno. **Not hardware-tested.** Not the original code. |
| `BringUpTest/` | New helper sketch to test each peripheral on the bench. Compiles; not hardware-tested. |

If the original sketch is found, add it as `firmware/original/` and keep it unmodified.

## Layout
```text
firmware/
├── FireFightingRobot/
│   ├── FireFightingRobot.ino   main state-machine firmware
│   ├── config.h                proposed pin map, tunable constants, shared types
│   └── secrets.h.example       copy to secrets.h and set the alert phone number
└── BringUpTest/
    └── BringUpTest.ino         serial-menu hardware check
```

## Build
Needs the Arduino AVR core and the built-in **Servo** and **SoftwareSerial** libraries.
```bash
arduino-cli core install arduino:avr
arduino-cli lib install Servo
arduino-cli compile --fqbn arduino:avr:uno firmware/FireFightingRobot
```
Compile check done for this commit: AVR GCC 7.3.0 with the Arduino AVR core 1.8.6, `-Wall -Wextra`, no warnings. FireFightingRobot: 10,306 bytes flash (31.5 %), 471 bytes static RAM (23 %). BringUpTest: 7,058 bytes flash (21.5 %), 359 bytes RAM (17.5 %). (Built with a manual AVR GCC command, not the Arduino IDE; compiling is not a hardware test.)

## Architecture
State machine: `Init → Patrol → Approach → Extinguish ⇄ Verify → Alert → Lockout → Patrol`, plus `GasAlarm`. Diagram and rationale: [`../docs/SOFTWARE_DOCUMENTATION.md`](../docs/SOFTWARE_DOCUMENTATION.md). Build and test steps: [`../docs/IMPLEMENTATION_GUIDE.md`](../docs/IMPLEMENTATION_GUIDE.md).
