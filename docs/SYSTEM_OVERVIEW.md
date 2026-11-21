# System Overview

> Sources: project report text and figures. Items not in the report are labelled *general theory* or *Proposed*. Component details marked dashed/yellow in diagrams are unverified.

## 1. Block diagram

Green = confirmed in the report's block diagram. Yellow/dashed = from the title, flowchart or Proteus schematic but absent from the report's block diagram (the GSM link).

```mermaid
flowchart LR
    subgraph IN["Sensing"]
        FL["Flame sensor - Left"]
        FF["Flame sensor - Forward"]
        FR["Flame sensor - Right"]
        GS["MQ-2 gas sensor"]
    end

    MCU["Arduino Uno<br/>(ATmega328P)"]

    subgraph DRV["Locomotion"]
        L293["L293D motor driver"]
        ML["Left DC motor"]
        MR["Right DC motor"]
    end

    subgraph EXT["Fire suppression"]
        SRV["Servo motor<br/>(aims water container)"]
        PUMP["5 V water pump"]
    end

    GSM["GSM module<br/>(SIM900-family shown in Proteus)"]

    PSU12["12 V supply"]
    PSU["Controller power supply"]

    FL --> MCU
    FF --> MCU
    FR --> MCU
    GS --> MCU
    MCU --> L293
    L293 --> ML
    L293 --> MR
    MCU --> SRV
    MCU --> PUMP
    MCU <-.-> GSM
    PSU12 --> L293
    PSU --> MCU

    classDef confirmed fill:#d9f2d9,stroke:#2e7d32,color:#000;
    classDef unverified fill:#fff3cd,stroke:#b58900,color:#000,stroke-dasharray: 4 3;
    class FL,FF,FR,GS,MCU,L293,ML,MR,SRV,PUMP,PSU12,PSU confirmed;
    class GSM unverified;
```

The report's original figure is kept for reference: [`../media/images/fig01_block_diagram_original.png`](../media/images/fig01_block_diagram_original.png).

## 2. Sensor → controller → actuator architecture

```mermaid
flowchart TB
    subgraph L1["Layer 1 - Sensing (inputs)"]
        S1["3x IR flame sensor modules<br/>(left / forward / right)"]
        S2["MQ-2 gas / smoke sensor"]
    end
    subgraph L2["Layer 2 - Processing"]
        C1["Arduino Uno<br/>decision logic (firmware not in repo)"]
    end
    subgraph L3["Layer 3 - Actuation (outputs)"]
        A1["L293D dual H-bridge"]
        A2["DC drive motors"]
        A3["Servo + 5 V water pump"]
    end
    subgraph L4["Layer 4 - Communication"]
        K1["GSM module: SMS / call alert"]
    end
    subgraph L5["Layer 5 - Power"]
        P1["12 V motor supply"]
        P2["Logic / controller supply"]
    end

    S1 --> C1
    S2 --> C1
    C1 --> A1 --> A2
    C1 --> A3
    C1 --> K1
    P1 --> A1
    P2 --> C1
```

## 3. Working principle

**Implemented per the report [R-text, R-flow]**

1. The robot starts moving and the three flame sensors are monitored.
2. If a flame sensor detects fire, the data goes to the Arduino.
3. The Arduino signals the motor driver and the robot stops.
4. The pump sprays water; the Arduino sends a message through the GSM module.
5. A delay passes and the cycle repeats.

```mermaid
flowchart TD
    A(["Power on"]) --> B["Initialise sensors, actuators<br/>and GSM module"]
    B --> C["Robot moves / scans<br/>(flowchart: 'Starts moving')"]
    C --> D{"Flame sensor<br/>detects fire?"}
    D -- "No" --> C
    D -- "Yes" --> E["Sensor data sent to Arduino"]
    E --> F["Arduino signals motor driver:<br/>robot stops"]
    F --> G["Pump extinguishes fire"]
    G --> H["Alert sent via GSM module<br/>(SMS shown in flowchart)"]
    H --> I["Delay"]
    I --> C

    N1["Not shown in original flowchart:<br/>MQ-2 branch, direction selection,<br/>servo aiming, call alert, re-check of flame"]
    H -.-> N1

    classDef note fill:#fff3cd,stroke:#b58900,color:#000,stroke-dasharray: 4 3;
    class N1 note;
```

**Not shown in the flowchart, so not claimed as implemented:** MQ-2 gas handling, selection of turn direction from the three sensors, servo aiming, a call alert, a "fire is out" check.

## 4. How the main subsystems work (general theory)

| Subsystem | Principle | Role in this project |
|---|---|---|
| IR flame sensor | A photodiode/phototransistor sensitive to the infrared emitted by flames (roughly 760–1100 nm). Typical modules add a comparator with a potentiometer to set the threshold, giving a digital output (and often an analog one). | Three modules (left/forward/right) give coarse direction. Model and output used are unverified. |
| Directional sensing | Each sensor sees a limited angle. Whichever sensor sees the flame indicates a side. | The report says three sensors "sense in which direction the fire is burning". The exact turning rule is not documented. |
| L293D | A dual H-bridge: two logic inputs per motor set the direction, an enable input gates or PWM-controls speed, and a separate motor supply powers the load. | Drives left and right DC motors from the Uno. |
| Servo + pump | A hobby servo moves to an angle set by a pulse width. The pump moves water when powered. | The container and pump sit on a servo so the spray direction can be changed [R-text]. |
| GSM module | A cellular modem controlled by AT commands over a serial link, e.g. text mode is selected with `AT+CMGF=1`, a message is sent with `AT+CMGS`, and a voice call is placed with `ATD`. | Sends the fire alert. The actual commands used by the project are unknown. |
| MQ-2 | A metal-oxide gas sensor whose resistance changes with combustible gas/smoke concentration, read as an analog voltage after heater warm-up. | Included for gas/smoke sensing. Its use in the control logic is undocumented. |

## 5. Why an Arduino Uno

The report names the Uno as the controller. A reasonable fit: it offers enough digital and analog pins for three flame inputs, one analog gas input, motor-driver control lines, a servo output and a serial link, and it is easy to program and to simulate in Proteus. Its limits (5 V pin current, one hardware serial port, 2 KB RAM) matter for GSM and servo use.
