# Testing and Validation

## Current status

| Level | Status |
|---|---|
| Proteus schematic | Exists (screenshot in the report); no simulation results recorded |
| Physical hardware | **No evidence in the repository** |
| Firmware compile check | Reference firmware compiles for the Uno (AVR GCC 7.3.0, Arduino AVR core 1.8.6); compile success is not a hardware test |

Firmware under test: the new reference firmware (`firmware/FireFightingRobot/`), which has only been compiled. Procedure for bench and system tests: [`IMPLEMENTATION_GUIDE.md`](IMPLEMENTATION_GUIDE.md).

**Every test below is Pending.** Nothing has been verified by evidence in this repository. When a test is run, record the actual result, the date, the setup and a photo/log/video reference, and only then change the status to Pass or Fail. Expected results describe the intended behaviour from the report's flowchart; thresholds and timings are not specified in the source and are left for the tester to define.

| Test ID | Test Description | Expected Result | Actual Result | Status |
|---|---|---|---|---|
| T-01 | Power-up and initialisation: apply power, observe startup | Controller boots, no unintended motor or pump motion | – | Pending |
| T-02 | Left flame detection: present a flame to the left sensor only | Left sensor output changes; robot reacts toward the left | – | Pending |
| T-03 | Front flame detection: flame at the forward sensor only | Forward sensor output changes; robot reacts forward | – | Pending |
| T-04 | Right flame detection: flame at the right sensor only | Right sensor output changes; robot reacts toward the right | – | Pending |
| T-05 | No-flame baseline: sensors with no fire, in ambient light and sunlight | No detection (records false-trigger behaviour) | – | Pending |
| T-06 | Motor direction: command forward, left, right | Wheels turn in the correct directions | – | Pending |
| T-07 | Motor stopping: detection occurs while moving | Motors stop per the flowchart | – | Pending |
| T-08 | Pump activation: flame detected and robot stopped | Pump runs and water exits toward the flame | – | Pending |
| T-09 | Servo aiming: sweep or point the water container | Servo moves smoothly without resetting the controller | – | Pending |
| T-10 | GSM SMS transmission: trigger an alert with a SIM | An SMS is received on the configured number | – | Pending |
| T-11 | GSM call functionality: trigger an alert call | Call is placed (only if implemented) | – | Pending |
| T-12 | MQ-2 response: expose to a safe test gas/smoke source | Reading changes after warm-up | – | Pending |
| T-13 | Power-supply stability: run motors, servo, pump and GSM together | Supply stays within tolerance, no brown-out | – | Pending |
| T-14 | Recovery after faults: remove the SIM/signal, power dip, flame removed | System recovers to normal operation without manual reset | – | Pending |
| T-15 | End-to-end demonstration: ignite a small, safely controlled flame at distance | Detect → stop → extinguish → alert | – | Pending |

## Test safety
Fire tests must be done with a small controlled flame, with an extinguisher at hand, away from flammables and under supervision.
