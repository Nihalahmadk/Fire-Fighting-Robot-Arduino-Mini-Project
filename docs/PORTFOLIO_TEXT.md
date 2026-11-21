# Portfolio Text

Wording below states only what the repository supports. **Before using it, confirm what you personally built and tested** (see "Check before use" at the end), then adjust.

## Resume (project entry)
**Fire Fighting Robot with SMS and Call Alert (Arduino Uno) – Academic ECE Mini-Project, 2023–24** *(team of 4)*
- Worked in a four-member team on an Arduino Uno-based fire-fighting robot concept combining three directional IR flame sensors, an MQ-2 gas sensor, an L293D-driven two-motor drive, a servo-mounted water pump and a GSM alert module.
- Documented the system architecture, operating flowchart and Proteus simulation schematic; produced the project report.
- Wrote a non-blocking Arduino state-machine firmware (flame debouncing, motor/pump interlocks, rate-limited GSM SMS/call alerts) as a reference implementation of the design; compile-verified for the Uno, hardware validation pending.
- Prepared the repository documentation: hardware audit, BOM, validation plan and limitations analysis.

## LinkedIn project description
A four-member ECE mini-project (2023–24) on a fire-fighting robot built around an Arduino Uno. Three flame sensors give left/forward/right fire direction, an L293D drives the motors, a servo-mounted pump sprays water, and a GSM module is intended to send SMS/call alerts. The design was documented with a block diagram, flowchart and Proteus schematic. This repository includes a technical audit, hardware documentation and a test plan. Skills: embedded systems, Arduino, sensor interfacing, motor-driver interfacing, GSM/AT commands, Proteus, technical documentation.

## GitHub profile blurb
Fire Fighting Robot (Arduino Uno): flame-sensor-directed robot with servo-aimed water pump and GSM SMS/call alert. Academic team project with audited documentation.

## 30-second interview introduction
"This was a four-person ECE mini-project: a small robot that finds a flame using three IR flame sensors, drives toward it with an L293D motor driver, sprays water from a servo-mounted pump, and sends a GSM alert. I [state your actual role]. I've since audited and documented it: I know exactly which parts we verified and which we didn't, and what I'd change to make it more robust, such as a proper pump driver, a separate supply for the GSM module and non-blocking timing."

Describe the reference firmware as written in 2026 for the repository, never as the code used in the 2023–24 project.

## Check before use
- Your actual role (hardware, code, simulation, report, testing).
- Whether a working prototype exists and was tested; do not say "tested" or "working" without evidence.
- Whether the call alert was actually implemented.
