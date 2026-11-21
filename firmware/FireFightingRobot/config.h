/*
 * config.h - Pin map, tunable constants and shared types
 *
 * REFERENCE IMPLEMENTATION (new work, not the original 2023-24 sketch).
 * All pin numbers are PROPOSED. Wire the robot to match this file, or change
 * the values here to match your wiring. Nothing in this file is verified
 * against the original hardware.
 *
 * Shared types live here (not in the .ino) because the Arduino build system
 * auto-generates function prototypes near the top of the sketch, before any
 * type declared inside it.
 */
#pragma once
#include <Arduino.h>

// ============================================================================
// Pin map (Arduino Uno, ATmega328P)  -- PROPOSED
// ============================================================================
// Flame sensor modules: digital output (DO). Most modules pull DO LOW on flame.
constexpr uint8_t PIN_FLAME_LEFT    = 2;
constexpr uint8_t PIN_FLAME_FORWARD = 3;
constexpr uint8_t PIN_FLAME_RIGHT   = 4;
constexpr uint8_t FLAME_ACTIVE_LEVEL = LOW;   // set HIGH if your module outputs HIGH on flame

// L293D: left motor = channel 1 (IN1/IN2/EN1), right motor = channel 2 (IN3/IN4/EN2)
constexpr uint8_t PIN_ML_IN1 = 7;
constexpr uint8_t PIN_ML_IN2 = 8;
constexpr uint8_t PIN_ML_EN  = 5;             // PWM (Timer0)
constexpr uint8_t PIN_MR_IN1 = 11;
constexpr uint8_t PIN_MR_IN2 = 12;
constexpr uint8_t PIN_MR_EN  = 6;             // PWM (Timer0)

constexpr uint8_t PIN_SERVO  = 10;            // water-container aiming servo
constexpr uint8_t PIN_PUMP   = A1;            // to pump driver (MOSFET/transistor/relay), NOT the pump itself
constexpr uint8_t PUMP_ON_LEVEL = HIGH;       // set LOW for an active-LOW relay module

constexpr uint8_t PIN_GAS    = A0;            // MQ-2 analog output (AO)

constexpr uint8_t PIN_GSM_RX = 9;             // Uno RX  <- GSM TX
constexpr uint8_t PIN_GSM_TX = A3;            // Uno TX  -> GSM RX (level-shift if the module is 3.3 V)
constexpr uint32_t GSM_BAUD  = 9600;

constexpr uint8_t PIN_STATUS_LED = LED_BUILTIN;

// D0/D1 are left free for USB serial debugging.

// ============================================================================
// Behaviour tuning  -- starting values; tune on the real robot
// ============================================================================
#define DEBUG_SERIAL 1                        // 1 = log to USB serial at 9600 baud

constexpr bool     PATROL_DRIVE_ENABLED = true;   // false = stand still and wait for flame

// Motors (PWM 0-255)
constexpr int16_t  PATROL_SPEED    = 120;
constexpr int16_t  APPROACH_SPEED  = 150;
constexpr int16_t  TURN_SPEED      = 140;
constexpr uint16_t MOTOR_DEADTIME_MS = 30;    // pause when reversing a motor (protects driver)

// Flame handling
constexpr uint16_t FLAME_SAMPLE_MS       = 20;
constexpr uint8_t  FLAME_CONFIRM_SAMPLES = 10;    // 10 x 20 ms = 200 ms of consistent readings
constexpr uint16_t FORWARD_REACHED_MS    = 1500;  // forward sensor active this long while driving => close enough
constexpr uint16_t FLAME_LOST_TIMEOUT_MS = 2000;  // flame invisible this long => back to patrol
constexpr uint32_t APPROACH_TIMEOUT_MS   = 20000; // give up approaching after this

// Extinguishing
constexpr uint32_t PUMP_RUN_MS       = 4000;  // one spray burst
constexpr uint16_t VERIFY_SETTLE_MS  = 1500;  // wait before re-checking for flame
constexpr uint8_t  MAX_EXTINGUISH_ATTEMPTS = 3;
constexpr uint8_t  SERVO_CENTER_DEG  = 90;
constexpr uint8_t  SERVO_SWEEP_DEG   = 30;    // +/- around center
constexpr uint8_t  SERVO_STEP_DEG    = 3;
constexpr uint16_t SERVO_STEP_MS     = 30;

// Lockout after an alert (robot stays stopped)
constexpr uint32_t LOCKOUT_EXTINGUISHED_MS = 5000;
constexpr uint32_t LOCKOUT_FAILED_MS       = 30000;

// MQ-2 (uncalibrated starting values: calibrate in clean air and with a safe test source)
constexpr uint32_t MQ2_WARMUP_MS      = 20000;
constexpr uint16_t GAS_SAMPLE_MS      = 200;
constexpr uint16_t GAS_ALARM_ADC      = 450;
constexpr uint16_t GAS_CLEAR_ADC      = 350;  // hysteresis
constexpr uint8_t  GAS_CONFIRM_SAMPLES = 5;

// GSM alerts
constexpr uint32_t GSM_BOOT_WAIT_MS   = 5000;     // module boot time after power-up
constexpr uint32_t FIRE_ALERT_MIN_INTERVAL_MS = 60000UL;
constexpr uint32_t GAS_ALERT_MIN_INTERVAL_MS  = 300000UL;
constexpr uint8_t  SMS_RETRIES        = 2;
constexpr bool     CALL_ENABLED       = true;     // ring the number after an unresolved-fire SMS
constexpr uint32_t CALL_RING_MS       = 20000;

// ============================================================================
// Shared types
// ============================================================================
enum class State : uint8_t {
  Init, Patrol, Approach, Extinguish, Verify, Alert, Lockout, GasAlarm
};

enum Outcome : uint8_t {
  OUTCOME_EXTINGUISHED,   // flame gone after spraying
  OUTCOME_STILL_BURNING,  // flame persists after all attempts
  OUTCOME_UNREACHABLE,    // could not reach the flame
  OUTCOME_GAS
};
