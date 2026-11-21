# Bill of Materials

Compiled from the project report and its figures only. **Model numbers are not invented**: where the source does not give one, the cell says so.

Verification: **Confirmed** = shown in report text or block diagram · **Unverified** = single or indirect source · **Proteus only** = appears only in the simulation schematic.

| Component | Qty | Purpose | Model | Verification |
|---|---:|---|---|---|
| Arduino Uno | 1 | Main controller | Uno (revision not stated) | Confirmed |
| IR flame sensor module (left) | 1 | Detect fire on the left | Not stated | Confirmed |
| IR flame sensor module (forward) | 1 | Detect fire ahead | Not stated | Confirmed |
| IR flame sensor module (right) | 1 | Detect fire on the right | Not stated | Confirmed |
| MQ-2 gas sensor | 1 | Gas / smoke sensing | MQ-2 (bare sensor or module not stated) | Confirmed |
| L293D motor driver | 1 | Drive the DC motors | L293D (IC or breakout not stated) | Confirmed |
| DC motors | 2 (block diagram) | Left / right drive | Not stated | Confirmed; Proteus shows 4, count unresolved |
| Servo motor | 1 | Aim the water container | Not stated | Confirmed |
| Water pump, 5 V | 1 | Spray water at the fire | Not stated | Confirmed |
| Water container | 1 | Carry water | – | Confirmed (report text) |
| GSM module | 1 | SMS / call alert | Proteus shows a SIM900D symbol; real module not stated | Existence confirmed; model **Unverified** |
| 12 V supply | 1 | Motor-driver supply | Battery/adapter not stated | Unverified |
| Controller power supply | 1 | Power the Arduino/logic | Not stated | Unverified |
| Relay (RL1) | 1 | Unknown | Not stated | Proteus only |
| Water-flow sensor | 1 | Unknown | Not stated | Proteus only |
| Chassis, wheels, SIM card, wiring | – | Mechanical / connectivity | Not documented | Not documented |
