# Project Audit

**Project:** Fire Fighting Robot with SMS and Call Alert Using Arduino
**Context:** ECE mini-project, academic year 2023–24, Hirasugar Institute of Technology, Nidasoshi (VTU Belagavi)
**Audit date:** 2026-11-21
**Audited material:** the original repository as it existed on `main` (2 commits, July 2025): `README.md` and the project report `.docx`.

> **Scope note:** this audit describes the repository as received (report + README only). The reference firmware, build guide and other additions made afterwards are listed in [`CHANGELOG.md`](CHANGELOG.md); they are new work, not original evidence.

## Status legend

| Label | Meaning |
|---|---|
| **Confirmed** | Stated in the report text or shown in a report figure |
| **Unverified** | Suggested by one source only, or model/value unknown |
| **Proposed** | A recommendation made during this audit; not part of the original project |
| **Not Implemented** | Mentioned only in generic or aspirational text; no evidence it was built |
| **Pending Validation** | No test evidence exists |

Sources are cited as: **[R-text]** report prose, **[R-block]** block-diagram figure, **[R-flow]** flowchart figure, **[R-proteus]** Proteus screenshot, **[README]** original README.

## 1. What was found in the repository

| Item | Present? |
|---|---|
| Project report (`.docx`) | Yes (now `report/original_project_report.docx`) |
| Arduino source code (`.ino/.cpp/.h`) | **No** |
| Proteus project file (`.pdsprj`) | **No** (only one screenshot inside the report) |
| Schematic / wiring files | **No** |
| Photographs of the physical robot | **No** |
| Demonstration video | **No** |
| License | **No** |

The report's 7 embedded images were extracted. Four are university/college logos and were not reused. Three are technical figures (block diagram, flowchart, Proteus schematic) and are in `media/images/`.

## 2. Project overview

A wheeled robot concept that senses fire with three flame sensors (left / forward / right), drives toward it through an L293D motor driver, stops, sprays water with a 5 V pump on a servo-aimed container, and notifies the owner through a GSM module [R-text, R-flow, README]. The stated aims are Arduino-to-sensor communication, flame-directed autonomous navigation and extinguishing, and a GSM SMS-and-call alert [R-text].

## 3. Hardware inventory

| Component | Evidence | Status | Notes |
|---|---|---|---|
| Arduino Uno | R-text, R-block, R-proteus | Confirmed | Board revision unknown |
| 3× flame sensor modules | R-text, R-block, R-proteus | Confirmed | Module model unknown; output type (digital/analog) not stated |
| MQ-2 gas sensor | R-block, R-proteus, report keywords | Confirmed | Not mentioned in the flowchart or algorithm logic that is project-specific |
| L293D motor driver | R-text ("L293 module"), R-block, R-proteus | Confirmed | IC vs. breakout module not stated |
| Left / right DC motors | R-block | Confirmed | Proteus draws four motor symbols on the L293D outputs |
| Servo motor | R-text, R-block | Confirmed | Carries the water container; model unknown |
| 5 V water pump | R-text, R-block | Confirmed | How it is switched is not documented |
| GSM module | README, R-flow, R-proteus | Confirmed (existence) / Unverified (model) | Proteus shows a SIM900D symbol |
| 12 V supply | R-block | Unverified | Source (battery/adapter) not stated |
| Controller power supply | R-block | Unverified | Not specified |
| Relay (RL1) | R-proteus only | Unverified | Not in report text or block diagram |
| Water-flow sensor | R-proteus only | Unverified | Not in report text or block diagram |
| Chassis, wheels, SIM card | none | Not documented | |

## 4. Software inventory

| Tool | Evidence | Status |
|---|---|---|
| Arduino IDE / Arduino language | R-text (Chapter 2) | Confirmed (named) |
| Proteus (schematic capture/simulation) | R-text ("Output in the Proteus Tool"), R-proteus | Confirmed |
| Firmware source | none | **Missing** |

## 5. System architecture and working principle

See [`SYSTEM_OVERVIEW.md`](SYSTEM_OVERVIEW.md) and [`diagrams/`](diagrams/). In short: three flame sensors and an MQ-2 feed the Uno; the Uno drives two motors through the L293D, aims a servo, switches the pump, and talks to a GSM module. The only project-specific logic description is the report flowchart: move → flame detected? → stop → pump + message → delay → repeat [R-flow].

## 6. Existing implementation

Nothing executable is available. The implementation can be described only at the block-diagram and flowchart level.

## 7. Missing information

1. Arduino firmware (any version).
2. Proteus project file and any recorded simulation run.
3. Pin assignments (the Proteus wiring is too dense to read reliably from the screenshot, and was deliberately **not** transcribed).
4. Exact models: flame sensor, GSM module, motors, servo, pump, batteries.
5. Whether a physical prototype was built and tested; photos and videos if so.
6. Whether the call alert is implemented (the flowchart shows only a message).
7. Role of the relay and flow sensor shown in Proteus.
8. Each team member's individual contribution.

## 8. Technical inconsistencies

| # | Inconsistency | Where |
|---|---|---|
| 1 | The abstract describes LM35 temperature control with an LCD on pin A1 and PC interfacing; none of these appear anywhere else. It appears to be text from a different project. | Abstract |
| 2 | The "Implementation and Working" outline lists thermal cameras, foam cannons, mapping/localisation and obstacle avoidance. Those are not in the block diagram, the Proteus design or the flowchart, and are beyond an Uno-based three-sensor design. | Ch. 3, algorithm list |
| 3 | The report's 7-step algorithm ("mapping and localization", "emergency stop and obstacle avoidance") does not match the 5-box flowchart. | Ch. 3 |
| 4 | The block diagram has no GSM module, though the title, aims and flowchart require one. | R-block |
| 5 | The Proteus schematic has a relay and a water-flow sensor that the block diagram and text do not mention. | R-proteus |
| 6 | The block diagram shows two motors; Proteus shows four motor symbols on the L293D plus one more near the flow sensor. | R-block vs R-proteus |
| 7 | The flowchart has no MQ-2 branch, no direction selection, no servo aiming and no "fire is out" check. | R-flow |
| 8 | Title and aims say "SMS and call"; the flowchart shows only a message. | Title, R-flow |
| 9 | "Result" is a Proteus schematic screenshot. It shows wiring, not simulation outputs, and no hardware result is reported. | Ch. 4 |
| 10 | The Proteus wiring was not traced to pin numbers in this audit. | R-proteus |

## 9. Limitations

See [`LIMITATIONS.md`](LIMITATIONS.md).

## 10. Recommended improvements

**Documentation / evidence (highest value first)**
1. Add the original `.ino` file(s) under `firmware/`.
2. Add the `.pdsprj` file and 2–3 screenshots of the simulation actually running.
3. Add photos and video of the physical robot, if it exists (checklist: [`../media/README.md`](../media/README.md)).
4. Fill in the pin map from the firmware or Proteus file.
5. Record test results in [`TESTING_AND_VALIDATION.md`](TESTING_AND_VALIDATION.md).

**Engineering (Proposed, if the project is revisited)**
- Switch the pump through a transistor or relay with a flyback diode instead of a direct pin.
- Power the GSM module from a supply that can deliver its transmit-burst current.
- Replace blocking `delay()` waits with `millis()` timing so sensors keep being read while the pump runs.
- Add a flame re-check after spraying and a pump run-time limit.
- Add a minimum interval between SMS alerts and a check for the module's response to each AT command.
- Debounce or filter flame readings, and use the MQ-2 only after its warm-up period.
