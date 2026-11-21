/*
 * FireFightingRobot.ino
 * Fire Fighting Robot with SMS and Call Alert - Arduino Uno
 *
 * *** REFERENCE IMPLEMENTATION ***
 * Newly written in 2026 from the design in the 2023-24 project report. It is
 * NOT the original project code (which is not available), and it has been
 * compile-checked only. It has not been tested on hardware. Pin numbers are
 * proposals defined in config.h.
 *
 * Behaviour (follows the report flowchart, with added safety logic):
 *   PATROL     drive forward slowly, watch three flame sensors
 *   APPROACH   steer toward the flame; stop when the forward sensor has seen
 *              it for a while
 *   EXTINGUISH stop, aim the servo, run the pump for a bounded time
 *   VERIFY     re-check the sensors; repeat up to a limit
 *   ALERT      send SMS (and optionally call) with the outcome
 *   LOCKOUT    stay stopped for a cooldown, then resume patrol
 *   GAS_ALARM  MQ-2 above threshold: stop everything and alert
 *
 * Design decisions (and why):
 *  - Non-blocking timing (millis) everywhere except the GSM exchange. The GSM
 *    exchange is blocking but bounded by timeouts, and only runs when motors
 *    and pump are already off, so no actuator is left running unattended and
 *    the supply is not asked to feed pump + GSM burst current together.
 *  - Flame inputs are debounced with a hysteresis counter (sunlight / flicker).
 *  - Motors reverse only through a dead-time stop; the pump refuses to start
 *    while a motor is moving (interlock) and is always forced off on entry to
 *    any state that does not need it.
 *  - Servo is attached only while spraying (the Servo library shares Timer1
 *    interrupts with SoftwareSerial and causes jitter/garbled bytes otherwise).
 *  - Gas alarm stops the robot: motors and relays can spark near gas.
 *  - SMS/call are rate-limited so a persistent fire cannot flood the phone.
 *  - No String objects (heap fragmentation on a 2 KB RAM device).
 */

#include <Servo.h>
#include <SoftwareSerial.h>
#include "config.h"

#if defined(__has_include)
#  if __has_include("secrets.h")
#    include "secrets.h"
#  endif
#endif
#ifndef ALERT_PHONE_NUMBER
#  define ALERT_PHONE_NUMBER "+000000000000"
#  define PLACEHOLDER_PHONE_NUMBER 1
#endif

#if DEBUG_SERIAL
#  define LOG(x) Serial.println(x)
#else
#  define LOG(x)
#endif

// ---------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------
Servo aimServo;
SoftwareSerial gsm(PIN_GSM_RX, PIN_GSM_TX);

State    state = State::Init;
uint32_t stateEnteredMs = 0;

// Flame sensing
const uint8_t FLAME_PINS[3] = {PIN_FLAME_LEFT, PIN_FLAME_FORWARD, PIN_FLAME_RIGHT};
const uint8_t FLAME_L = 1, FLAME_F = 2, FLAME_R = 4;
uint8_t  flameCounter[3] = {0, 0, 0};
uint8_t  flameStableMask = 0;
uint32_t lastFlameSampleMs = 0;
uint32_t lastFlameSeenMs = 0;
uint32_t forwardSinceMs = 0;           // 0 = forward sensor not currently active

// Motors: index 0 = left, 1 = right
const uint8_t MOTOR_IN1[2] = {PIN_ML_IN1, PIN_MR_IN1};
const uint8_t MOTOR_IN2[2] = {PIN_ML_IN2, PIN_MR_IN2};
const uint8_t MOTOR_EN[2]  = {PIN_ML_EN,  PIN_MR_EN};
int16_t  motorTarget[2]  = {0, 0};
int16_t  motorApplied[2] = {0, 0};
uint32_t motorChangedMs[2] = {0, 0};

// Pump / servo
bool     pumpOn = false;
uint32_t pumpStartMs = 0;
uint8_t  extinguishAttempts = 0;
int16_t  servoAngle = SERVO_CENTER_DEG;
int8_t   servoDir = 1;
uint32_t lastServoStepMs = 0;

// Gas
uint16_t gasAvg = 0;
bool     gasAlarmActive = false;
uint8_t  gasCounter = 0;
uint32_t lastGasSampleMs = 0;

// GSM / alerts
bool     gsmReady = false;
bool     gsmInitTried = false;
Outcome  outcome = OUTCOME_EXTINGUISHED;
uint32_t lockoutMs = 0;
bool     fireAlertEverSent = false, gasAlertEverSent = false;
uint32_t lastFireAlertMs = 0, lastGasAlertMs = 0;

// Status LED
uint32_t lastLedMs = 0;

// ---------------------------------------------------------------------------
// Motor control
// ---------------------------------------------------------------------------
static void applyMotor(uint8_t i, int16_t speed) {
  if (speed > 255)  speed = 255;
  if (speed < -255) speed = -255;
  if (speed > 0)      { digitalWrite(MOTOR_IN1[i], HIGH); digitalWrite(MOTOR_IN2[i], LOW);  }
  else if (speed < 0) { digitalWrite(MOTOR_IN1[i], LOW);  digitalWrite(MOTOR_IN2[i], HIGH); }
  else                { digitalWrite(MOTOR_IN1[i], LOW);  digitalWrite(MOTOR_IN2[i], LOW);  }
  analogWrite(MOTOR_EN[i], speed < 0 ? -speed : speed);
  motorApplied[i] = speed;
}

// Immediate stop of both motors (used for safety stops).
static void stopMotorsNow() {
  for (uint8_t i = 0; i < 2; i++) {
    motorTarget[i] = 0;
    applyMotor(i, 0);
    motorChangedMs[i] = millis();
  }
}

static void setDrive(int16_t left, int16_t right) {
  motorTarget[0] = left;
  motorTarget[1] = right;
}

// Moves each motor toward its target; forces a brief stop when direction flips.
static void motorsUpdate(uint32_t now) {
  for (uint8_t i = 0; i < 2; i++) {
    if (motorApplied[i] == motorTarget[i]) continue;
    bool reversing = (motorApplied[i] > 0 && motorTarget[i] < 0) ||
                     (motorApplied[i] < 0 && motorTarget[i] > 0);
    if (reversing) {
      applyMotor(i, 0);
      motorChangedMs[i] = now;
      continue;
    }
    if (motorApplied[i] == 0 && motorTarget[i] != 0 &&
        (now - motorChangedMs[i]) < MOTOR_DEADTIME_MS) continue;
    applyMotor(i, motorTarget[i]);
    motorChangedMs[i] = now;
  }
}

// ---------------------------------------------------------------------------
// Pump and servo
// ---------------------------------------------------------------------------
static void pumpSet(bool on) {
  if (on) {
    // Interlock: never start the pump while a wheel is turning.
    if (motorApplied[0] != 0 || motorApplied[1] != 0) {
      LOG(F("Pump refused: motors moving"));
      return;
    }
    digitalWrite(PIN_PUMP, PUMP_ON_LEVEL);
    if (!pumpOn) pumpStartMs = millis();
    pumpOn = true;
  } else {
    digitalWrite(PIN_PUMP, !PUMP_ON_LEVEL);
    pumpOn = false;
  }
}

static void servoStart() {
  if (!aimServo.attached()) aimServo.attach(PIN_SERVO);
  servoAngle = SERVO_CENTER_DEG;
  servoDir = 1;
  aimServo.write(servoAngle);
}

static void servoSweepUpdate(uint32_t now) {
  if (!aimServo.attached() || (now - lastServoStepMs) < SERVO_STEP_MS) return;
  lastServoStepMs = now;
  servoAngle += servoDir * SERVO_STEP_DEG;
  if (servoAngle >= SERVO_CENTER_DEG + SERVO_SWEEP_DEG) { servoAngle = SERVO_CENTER_DEG + SERVO_SWEEP_DEG; servoDir = -1; }
  if (servoAngle <= SERVO_CENTER_DEG - SERVO_SWEEP_DEG) { servoAngle = SERVO_CENTER_DEG - SERVO_SWEEP_DEG; servoDir = 1; }
  aimServo.write(servoAngle);
}

static void servoCenter() {
  if (aimServo.attached()) aimServo.write(SERVO_CENTER_DEG);
}

static void servoRelease() {
  if (aimServo.attached()) aimServo.detach();
}

// ---------------------------------------------------------------------------
// Sensors
// ---------------------------------------------------------------------------
// Debounce with hysteresis: counter rises while a sensor reads "flame" and
// falls otherwise; the stable flag sets at CONFIRM and clears at zero.
static void updateFlameSensors(uint32_t now) {
  if ((now - lastFlameSampleMs) < FLAME_SAMPLE_MS) return;
  lastFlameSampleMs = now;
  for (uint8_t i = 0; i < 3; i++) {
    bool active = (digitalRead(FLAME_PINS[i]) == FLAME_ACTIVE_LEVEL);
    if (active) { if (flameCounter[i] < FLAME_CONFIRM_SAMPLES) flameCounter[i]++; }
    else        { if (flameCounter[i] > 0) flameCounter[i]--; }
    uint8_t bit = 1 << i;
    if (flameCounter[i] >= FLAME_CONFIRM_SAMPLES) flameStableMask |= bit;
    else if (flameCounter[i] == 0)                flameStableMask &= ~bit;
  }
  if (flameStableMask) lastFlameSeenMs = now;
}

static void updateGasSensor(uint32_t now) {
  if (now < MQ2_WARMUP_MS) return;               // heater needs warm-up
  if ((now - lastGasSampleMs) < GAS_SAMPLE_MS) return;
  lastGasSampleMs = now;
  uint16_t adc = analogRead(PIN_GAS);
  gasAvg = (gasAvg == 0) ? adc : (uint16_t)((3UL * gasAvg + adc) / 4);   // light smoothing
  if (gasAvg >= GAS_ALARM_ADC) {
    if (gasCounter < GAS_CONFIRM_SAMPLES) gasCounter++;
    if (gasCounter >= GAS_CONFIRM_SAMPLES) gasAlarmActive = true;
  } else if (gasAvg <= GAS_CLEAR_ADC) {
    gasCounter = 0;
    gasAlarmActive = false;
  }
}

// ---------------------------------------------------------------------------
// GSM (AT commands). Blocking, but every wait is bounded by a timeout.
// ---------------------------------------------------------------------------
static void gsmFlush() { while (gsm.available()) gsm.read(); }

// Waits for tokenA or tokenB in the incoming data.
// Returns 1 (A), 2 (B), -1 (ERROR received) or 0 (timeout).
static int8_t gsmWaitFor(const char* tokenA, uint32_t timeoutMs, const char* tokenB) {
  char buf[48];
  uint8_t n = 0;
  buf[0] = '\0';
  uint32_t start = millis();
  while ((millis() - start) < timeoutMs) {
    while (gsm.available()) {
      char c = (char)gsm.read();
      if (n >= sizeof(buf) - 1) {                // rolling window: drop the oldest half
        uint8_t half = sizeof(buf) / 2;
        memmove(buf, buf + half, n - half);
        n -= half;
      }
      buf[n++] = c;
      buf[n] = '\0';
      if (strstr(buf, tokenA)) return 1;
      if (tokenB && strstr(buf, tokenB)) return 2;
      if (strstr(buf, "ERROR")) return -1;
    }
  }
  return 0;
}

static bool gsmCommand(const __FlashStringHelper* cmd, const char* expect, uint32_t timeoutMs) {
  gsmFlush();
  gsm.println(cmd);
  return gsmWaitFor(expect, timeoutMs, nullptr) == 1;
}

static bool gsmInit() {
  LOG(F("GSM: init"));
  bool alive = false;
  for (uint8_t i = 0; i < 5 && !alive; i++) {
    alive = gsmCommand(F("AT"), "OK", 1000);
    if (!alive) delay(500);                      // boot-time only; nothing is moving yet
  }
  if (!alive) { LOG(F("GSM: no response")); return false; }
  gsmCommand(F("ATE0"), "OK", 1000);             // echo off
  if (!gsmCommand(F("AT+CMGF=1"), "OK", 1000)) { LOG(F("GSM: text mode failed")); return false; }

  for (uint8_t i = 0; i < 10; i++) {             // wait for network registration (home/roaming)
    gsmFlush();
    gsm.println(F("AT+CREG?"));
    if (gsmWaitFor(",1", 1000, ",5") > 0) { LOG(F("GSM: registered")); return true; }
    delay(1000);
  }
  LOG(F("GSM: not registered"));
  return false;
}

static void gsmPrintMessage(Outcome o) {
  switch (o) {
    case OUTCOME_EXTINGUISHED:  gsm.print(F("FIRE ALERT: flame detected and extinguished by robot."));       break;
    case OUTCOME_STILL_BURNING: gsm.print(F("FIRE ALERT: flame still detected after extinguishing. Check now.")); break;
    case OUTCOME_UNREACHABLE:   gsm.print(F("FIRE ALERT: flame detected, robot could not reach it. Check now."));  break;
    case OUTCOME_GAS:           gsm.print(F("GAS ALERT: gas/smoke level high. Robot stopped."));              break;
  }
}

static bool sendSmsOnce(Outcome o) {
  gsmFlush();
  gsm.print(F("AT+CMGS=\""));
  gsm.print(F(ALERT_PHONE_NUMBER));
  gsm.println('"');
  if (gsmWaitFor(">", 5000, nullptr) != 1) return false;
  gsmPrintMessage(o);
  gsm.write((uint8_t)0x1A);                      // Ctrl+Z ends the message
  return gsmWaitFor("+CMGS:", 30000, nullptr) == 1;
}

static bool sendSms(Outcome o) {
#ifdef PLACEHOLDER_PHONE_NUMBER
  LOG(F("WARNING: secrets.h missing, using placeholder phone number"));
#endif
  if (!gsmReady) gsmReady = gsmInit();           // retry initialisation if it failed earlier
  if (!gsmReady) return false;
  for (uint8_t i = 0; i <= SMS_RETRIES; i++) {
    if (sendSmsOnce(o)) { LOG(F("SMS sent")); return true; }
    LOG(F("SMS failed, retrying"));
    delay(1000);
  }
  gsmReady = false;                              // force re-init on next alert
  return false;
}

static void placeCall() {
  if (!gsmReady) return;
  gsmFlush();
  gsm.print(F("ATD"));
  gsm.print(F(ALERT_PHONE_NUMBER));
  gsm.println(';');                              // ';' = voice call
  uint32_t start = millis();
  while ((millis() - start) < CALL_RING_MS) { }  // ring; bounded wait, actuators are off
  gsm.println(F("ATH"));                         // hang up
  gsmWaitFor("OK", 2000, nullptr);
  LOG(F("Call placed"));
}

// Sends the alert for an outcome, honouring the minimum interval between alerts.
static void sendAlert(Outcome o) {
  uint32_t now = millis();
  bool isGas = (o == OUTCOME_GAS);
  bool everSent = isGas ? gasAlertEverSent : fireAlertEverSent;
  uint32_t last = isGas ? lastGasAlertMs : lastFireAlertMs;
  uint32_t minGap = isGas ? GAS_ALERT_MIN_INTERVAL_MS : FIRE_ALERT_MIN_INTERVAL_MS;
  if (everSent && (now - last) < minGap) { LOG(F("Alert suppressed (rate limit)")); return; }

  bool ok = sendSms(o);
  if (ok && CALL_ENABLED && o != OUTCOME_EXTINGUISHED) placeCall();
  if (isGas) { gasAlertEverSent = true; lastGasAlertMs = millis(); }
  else       { fireAlertEverSent = true; lastFireAlertMs = millis(); }
}

// ---------------------------------------------------------------------------
// State machine
// ---------------------------------------------------------------------------
static void enterState(State s) {
  state = s;
  stateEnteredMs = millis();
  switch (s) {
    case State::Init:
      break;
    case State::Patrol:
      pumpSet(false); servoRelease();
      extinguishAttempts = 0;
      forwardSinceMs = 0;
      LOG(F("-> PATROL"));
      break;
    case State::Approach:
      forwardSinceMs = 0;
      LOG(F("-> APPROACH"));
      break;
    case State::Extinguish:
      stopMotorsNow();                           // interlock: wheels stopped before the pump runs
      servoStart();
      extinguishAttempts++;
      pumpSet(true);
      LOG(F("-> EXTINGUISH"));
      break;
    case State::Verify:
      pumpSet(false);
      servoCenter();
      LOG(F("-> VERIFY"));
      break;
    case State::Alert:
      stopMotorsNow(); pumpSet(false); servoRelease();
      LOG(F("-> ALERT"));
      sendAlert(outcome);                        // blocking but bounded; actuators are off
      lockoutMs = (outcome == OUTCOME_EXTINGUISHED) ? LOCKOUT_EXTINGUISHED_MS : LOCKOUT_FAILED_MS;
      enterState(State::Lockout);
      break;
    case State::Lockout:
      LOG(F("-> LOCKOUT"));
      break;
    case State::GasAlarm:
      stopMotorsNow(); pumpSet(false); servoRelease();
      LOG(F("-> GAS ALARM"));
      sendAlert(OUTCOME_GAS);
      break;
  }
}

static void runStateMachine(uint32_t now) {
  switch (state) {

    case State::Init:
      // Stay stationary while the GSM module boots and the MQ-2 heater warms up.
      if (!gsmInitTried && now >= GSM_BOOT_WAIT_MS) {
        gsmReady = gsmInit();
        gsmInitTried = true;
      }
      if (gsmInitTried) enterState(State::Patrol);
      break;

    case State::Patrol:
      if (gasAlarmActive) { enterState(State::GasAlarm); break; }
      if (flameStableMask) { enterState(State::Approach); break; }
      if (PATROL_DRIVE_ENABLED) setDrive(PATROL_SPEED, PATROL_SPEED);
      else                      setDrive(0, 0);
      break;

    case State::Approach: {
      if (gasAlarmActive) { enterState(State::GasAlarm); break; }
      if ((now - stateEnteredMs) > APPROACH_TIMEOUT_MS) {
        stopMotorsNow(); outcome = OUTCOME_UNREACHABLE; enterState(State::Alert); break;
      }
      if (!flameStableMask) {                    // lost sight of the flame
        setDrive(0, 0);
        forwardSinceMs = 0;
        if ((now - lastFlameSeenMs) > FLAME_LOST_TIMEOUT_MS) enterState(State::Patrol);
        break;
      }
      bool left  = flameStableMask & FLAME_L;
      bool fwd   = flameStableMask & FLAME_F;
      bool right = flameStableMask & FLAME_R;
      if (fwd || (left && right)) {
        if (forwardSinceMs == 0) forwardSinceMs = now;
        setDrive(APPROACH_SPEED, APPROACH_SPEED);
        if ((now - forwardSinceMs) >= FORWARD_REACHED_MS) { enterState(State::Extinguish); }
      } else if (left) {
        forwardSinceMs = 0;
        setDrive(-TURN_SPEED, TURN_SPEED);       // pivot left
      } else if (right) {
        forwardSinceMs = 0;
        setDrive(TURN_SPEED, -TURN_SPEED);       // pivot right
      }
      break;
    }

    case State::Extinguish:
      servoSweepUpdate(now);
      if ((now - pumpStartMs) >= PUMP_RUN_MS) enterState(State::Verify);
      break;

    case State::Verify:
      if ((now - stateEnteredMs) < VERIFY_SETTLE_MS) break;
      servoRelease();
      if (!flameStableMask) {
        outcome = OUTCOME_EXTINGUISHED; enterState(State::Alert);
      } else if (extinguishAttempts < MAX_EXTINGUISH_ATTEMPTS) {
        enterState(State::Extinguish);
      } else {
        outcome = OUTCOME_STILL_BURNING; enterState(State::Alert);
      }
      break;

    case State::Alert:                           // handled on entry
      break;

    case State::Lockout:
      if ((now - stateEnteredMs) >= lockoutMs) enterState(State::Patrol);
      break;

    case State::GasAlarm:
      if (!gasAlarmActive) enterState(State::Patrol);
      break;
  }
}

// ---------------------------------------------------------------------------
// Status LED: slow blink = patrol, fast = approach, solid = spraying, very fast = alert/lockout/gas
// ---------------------------------------------------------------------------
static void updateStatusLed(uint32_t now) {
  uint16_t interval;
  switch (state) {
    case State::Patrol:     interval = 500; break;
    case State::Approach:   interval = 150; break;
    case State::Extinguish:
    case State::Verify:     digitalWrite(PIN_STATUS_LED, HIGH); return;
    case State::Init:       interval = 1000; break;
    default:                interval = 60;  break;
  }
  if ((now - lastLedMs) >= interval) {
    lastLedMs = now;
    digitalWrite(PIN_STATUS_LED, !digitalRead(PIN_STATUS_LED));
  }
}

// ---------------------------------------------------------------------------
// Arduino entry points
// ---------------------------------------------------------------------------
void setup() {
  // Safe outputs first: set the OFF level before switching the pin to OUTPUT.
  digitalWrite(PIN_PUMP, !PUMP_ON_LEVEL);
  pinMode(PIN_PUMP, OUTPUT);
  for (uint8_t i = 0; i < 2; i++) {
    digitalWrite(MOTOR_IN1[i], LOW);
    digitalWrite(MOTOR_IN2[i], LOW);
    digitalWrite(MOTOR_EN[i],  LOW);
    pinMode(MOTOR_IN1[i], OUTPUT);
    pinMode(MOTOR_IN2[i], OUTPUT);
    pinMode(MOTOR_EN[i],  OUTPUT);
  }
  for (uint8_t i = 0; i < 3; i++) pinMode(FLAME_PINS[i], INPUT);
  pinMode(PIN_STATUS_LED, OUTPUT);

#if DEBUG_SERIAL
  Serial.begin(9600);
#endif
  gsm.begin(GSM_BAUD);
  LOG(F("Fire Fighting Robot - reference firmware"));
  enterState(State::Init);
}

void loop() {
  uint32_t now = millis();
  updateFlameSensors(now);
  updateGasSensor(now);
  runStateMachine(now);
  motorsUpdate(now);
  updateStatusLed(now);
}
