/*
 * BringUpTest.ino - one-peripheral-at-a-time hardware check
 *
 * Use this BEFORE the main firmware. Open the Serial Monitor at 9600 baud
 * (line ending: "No line ending") and send a single character:
 *   f  stream the three flame sensors (1 = flame) for 5 s
 *   g  stream the MQ-2 analog value for 5 s
 *   w  left+right motors forward 1 s      s  both reverse 1 s
 *   l  pivot left 1 s                     r  pivot right 1 s
 *   p  pump on for 1 s (container must hold water)
 *   v  servo sweep once
 *   a  GSM pass-through: type AT commands in the monitor (reset board to exit)
 * Pins come from the main firmware's config.h so both sketches stay in sync.
 * Reference/test code: not the original project code; not hardware-tested.
 */
#include <Servo.h>
#include <SoftwareSerial.h>
#include "../FireFightingRobot/config.h"

Servo aimServo;
SoftwareSerial gsm(PIN_GSM_RX, PIN_GSM_TX);

const uint8_t IN1[2] = {PIN_ML_IN1, PIN_MR_IN1};
const uint8_t IN2[2] = {PIN_ML_IN2, PIN_MR_IN2};
const uint8_t EN[2]  = {PIN_ML_EN,  PIN_MR_EN};

static void drive(int16_t l, int16_t r, uint16_t ms) {
  int16_t v[2] = {l, r};
  for (uint8_t i = 0; i < 2; i++) {
    digitalWrite(IN1[i], v[i] > 0);
    digitalWrite(IN2[i], v[i] < 0);
    analogWrite(EN[i], abs(v[i]));
  }
  delay(ms);
  for (uint8_t i = 0; i < 2; i++) { digitalWrite(IN1[i], LOW); digitalWrite(IN2[i], LOW); analogWrite(EN[i], 0); }
}

void setup() {
  digitalWrite(PIN_PUMP, !PUMP_ON_LEVEL);
  pinMode(PIN_PUMP, OUTPUT);
  for (uint8_t i = 0; i < 2; i++) { pinMode(IN1[i], OUTPUT); pinMode(IN2[i], OUTPUT); pinMode(EN[i], OUTPUT); }
  pinMode(PIN_FLAME_LEFT, INPUT); pinMode(PIN_FLAME_FORWARD, INPUT); pinMode(PIN_FLAME_RIGHT, INPUT);
  Serial.begin(9600);
  gsm.begin(GSM_BAUD);
  Serial.println(F("Bring-up ready. Commands: f g w s l r p v a"));
}

void loop() {
  if (!Serial.available()) return;
  char c = Serial.read();
  switch (c) {
    case 'f':
      for (uint8_t i = 0; i < 25; i++) {
        Serial.print(F("L=")); Serial.print(digitalRead(PIN_FLAME_LEFT) == FLAME_ACTIVE_LEVEL);
        Serial.print(F(" F=")); Serial.print(digitalRead(PIN_FLAME_FORWARD) == FLAME_ACTIVE_LEVEL);
        Serial.print(F(" R=")); Serial.println(digitalRead(PIN_FLAME_RIGHT) == FLAME_ACTIVE_LEVEL);
        delay(200);
      }
      break;
    case 'g':
      for (uint8_t i = 0; i < 25; i++) { Serial.print(F("MQ2 ADC=")); Serial.println(analogRead(PIN_GAS)); delay(200); }
      break;
    case 'w': drive(PATROL_SPEED, PATROL_SPEED, 1000); break;
    case 's': drive(-PATROL_SPEED, -PATROL_SPEED, 1000); break;
    case 'l': drive(-TURN_SPEED, TURN_SPEED, 1000); break;
    case 'r': drive(TURN_SPEED, -TURN_SPEED, 1000); break;
    case 'p':
      digitalWrite(PIN_PUMP, PUMP_ON_LEVEL); delay(1000); digitalWrite(PIN_PUMP, !PUMP_ON_LEVEL);
      break;
    case 'v':
      aimServo.attach(PIN_SERVO);
      for (int a = SERVO_CENTER_DEG - SERVO_SWEEP_DEG; a <= SERVO_CENTER_DEG + SERVO_SWEEP_DEG; a += 2) { aimServo.write(a); delay(30); }
      aimServo.write(SERVO_CENTER_DEG); delay(400); aimServo.detach();
      break;
    case 'a':
      Serial.println(F("GSM pass-through. Reset the board to exit."));
      for (;;) {
        if (Serial.available()) gsm.write(Serial.read());
        if (gsm.available())    Serial.write(gsm.read());
      }
  }
}
