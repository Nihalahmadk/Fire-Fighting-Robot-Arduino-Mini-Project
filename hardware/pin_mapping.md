# Pin Mapping

Two tables follow: the **original project's** pin mapping (not known) and the **proposed reference build** (new, used by the reference firmware).

## A. Original project (2023–24)

**Status: no pin assignment is verified.** The repository has no firmware, and the Proteus wiring was not transcribed because it cannot be traced reliably from the screenshot. Rather than guess, every pin below is marked as unknown.

| Arduino pin | Connected component | Signal type | Direction | Description | Verification |
|---|---|---|---|---|---|
| Unknown | Flame sensor – left | Not stated (digital or analog) | Input | Left fire detection | Unverified |
| Unknown | Flame sensor – forward | Not stated | Input | Forward fire detection | Unverified |
| Unknown | Flame sensor – right | Not stated | Input | Right fire detection | Unverified |
| Unknown | MQ-2 | Not stated | Input | Gas level | Unverified |
| Unknown (×2–4) | L293D inputs (IN1–IN4) | Digital | Output | Motor direction | Unverified |
| Unknown (0–2) | L293D enable pins | Digital / PWM | Output | Motor enable / speed | Unverified |
| Unknown | Servo signal | PWM | Output | Water-container angle | Unverified |
| Unknown | Pump driver (direct, relay or transistor) | Digital | Output | Pump on/off | Unverified |
| Unknown (2 pins) | GSM module TX/RX | UART (hardware or software serial) | Bidirectional | AT commands | Unverified |

## B. Proposed pin map for the reference build (NOT the original project)

Defined in [`../firmware/FireFightingRobot/config.h`](../firmware/FireFightingRobot/config.h). Status of every row: **Proposed; not hardware-tested.**

| Arduino pin | Connected component | Signal type | Direction | Description |
|---|---|---|---|---|
| D2 | Flame sensor – left (DO) | Digital | Input | Left fire detection |
| D3 | Flame sensor – forward (DO) | Digital | Input | Forward fire detection |
| D4 | Flame sensor – right (DO) | Digital | Input | Right fire detection |
| A0 | MQ-2 (AO) | Analog | Input | Gas/smoke level |
| D7 | L293D IN1 | Digital | Output | Left motor direction |
| D8 | L293D IN2 | Digital | Output | Left motor direction |
| D5 | L293D EN1 | PWM | Output | Left motor speed |
| D11 | L293D IN3 | Digital | Output | Right motor direction |
| D12 | L293D IN4 | Digital | Output | Right motor direction |
| D6 | L293D EN2 | PWM | Output | Right motor speed |
| D10 | Servo signal | PWM (Servo library) | Output | Aims the water container |
| A1 | Pump driver input | Digital | Output | Pump on/off (via MOSFET/relay) |
| D9 | GSM TX | SoftwareSerial RX | Input | GSM responses |
| A3 | GSM RX | SoftwareSerial TX | Output | AT commands |
| D13 | On-board LED | Digital | Output | Status |
| D0 / D1 | USB serial | UART | – | Debug, left free |

Pin choices avoid D13 for the pump (it pulses at boot) and keep PWM motor-enable pins on Timer0 so that the Servo library's use of Timer1 does not affect them.

## How to complete table A

1. From the original sketch: read the `#define` / `const int` / `pinMode()` lines, or
2. From the Proteus file: open the design and read each wire's Arduino pin label.

Then change each row's verification to **Confirmed** and cite the source.

## Engineering considerations (general; not claims about the original build)

| Topic | Consideration |
|---|---|
| Arduino I/O | Each pin is limited to roughly 20 mA recommended (40 mA absolute maximum). Motors, pump and servo must never be powered directly from a pin. |
| Motor driver | An L293D handles about 600 mA continuous per channel (1.2 A peak) with a 4.5–36 V motor supply; stalled motors can exceed this. Check the motors' stall current. |
| Power | The 5 V rail of an Uno cannot support a servo, pump and GSM module together. Use separate regulated supplies for high-current loads. |
| Common ground | All supplies (Arduino, motors, GSM, pump) must share a ground reference. |
| GSM current | GSM modules in the SIM800/SIM900 family draw short current bursts of up to about 2 A when transmitting. A weak supply causes resets and failed sends. Check the actual module's datasheet. |
| Pump driver | An inductive load needs a transistor or relay with a flyback diode. |
| Sensor placement | Flame sensors have a limited angular field; three sensors leave blind spots. |
| Serial pins | Using the Uno's hardware serial (pins 0/1) for the GSM module conflicts with USB programming and debugging. A software-serial or separate port avoids this. |
