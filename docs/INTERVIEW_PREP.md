# Interview Preparation

Questions an embedded/firmware interviewer could ask about this project, with what a strong answer covers. Answer only from what you actually did.

## Design
1. **Why an Arduino Uno?** Pin count, ease of programming and simulation; mention its limits (one UART, 5 V pin current, 2 KB RAM).
2. **How do you tell where the fire is?** Three sensors with different directions; the active one picks the turn. Explain blind spots and the limited field of view.
3. **How does an IR flame sensor work?** IR-sensitive photodiode, comparator and threshold potentiometer; digital/analog output; sunlight interference.
4. **Digital or analog output, and why?** State what you used. Digital is simple; analog allows thresholds and estimated closeness.

## Motors and power
5. **How does the L293D control a DC motor?** H-bridge, direction inputs, enable/PWM, separate motor supply, current limits.
6. **Why not drive the pump or motors from Arduino pins?** 20/40 mA pin limit; need drivers; flyback protection.
7. **How is the pump switched in your design?** Be exact; the Proteus schematic has a relay, so know why.
8. **What happens when the pump, servo and GSM draw current together?** Brown-out; separate supplies; common ground.

## GSM
9. **How does the GSM module send an SMS?** Serial AT commands: set text mode, specify recipient, send text, terminate with Ctrl+Z; check responses.
10. **What can go wrong?** No network, no SIM credit, supply brown-out, unchecked response, repeated messages.
11. **Is the call alert implemented?** Answer honestly.

## Firmware
12. **Walk through your control loop.** Initialise → read sensors → decide → move/stop → pump → alert → delay.
13. **What are the problems with `delay()` here?** Blocks sensor reading and alert handling; use `millis()` state timing.
14. **How would you reduce false triggers?** Debounce, multiple consecutive samples, ambient-light calibration, combining with MQ-2.
15. **How would you know the fire is out?** Re-read sensors after spraying, retry limit.

## Reference firmware (be ready to defend it)
- **Why a state machine?** Clear transitions, bounded timing, easy to test and extend.
- **Why is the GSM exchange blocking?** Sequential AT protocol, bounded by timeouts, and only with motors and pump off; the trade-off is that sensors are not read for up to about 30 s.
- **Why detach the servo?** The Servo library uses Timer1 interrupts and disturbs SoftwareSerial.
- **Why a pump/motor interlock and dead-time?** Supply sag and driver protection.
- **How do you estimate distance with digital flame sensors?** Time-based; mention the weakness and propose analog readings or an ultrasonic sensor.

## Testing and honesty
16. **How did you test it?** Say exactly what was simulated and what was physical.
17. **Where is your code?** Say the original sketch was not preserved, and that the firmware in the repo is a reference implementation you wrote later from the design, with proposed pins and no hardware test yet. Then walk through its state machine.
18. **What was your contribution in a team of four?** Give specific tasks.
19. **What would you change in a second version?** Pump driver, power design, non-blocking firmware, obstacle sensing, better flame sensing (analog or camera), fault handling.
