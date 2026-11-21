# Simulation

## What exists
One screenshot of a **Proteus schematic**, embedded in the report's "Result" section (the report captions it "Output in the Proteus Tool"). It is at [`../media/images/fig03_proteus_schematic.jpg`](../media/images/fig03_proteus_schematic.jpg).

It shows: Arduino Uno, three flame-sensor modules, an MQ-2, an L293D with motor symbols, a SIM900D GSM symbol, a relay (RL1) and a water-flow sensor.

## What does not exist
- The Proteus project file (`.pdsprj`).
- Any captured simulation run (sensor stimulus, motor/pump response, serial terminal output).
- The firmware that was loaded into the simulated Uno.

## To add
1. `simulation/FireFightingRobot.pdsprj`
2. Screenshots of the simulation running, named `sim_<scenario>.png` (e.g. `sim_flame_left.png`).
3. The firmware `.hex` or `.ino` used, if not in `firmware/`.

A simulation is not hardware validation, and the documentation treats it that way.
