#include <Arduino.h>

// ==================== PIN CONFIGURATION ====================
// Arduino board pins for motor control
#define PWM_PIN 6          // PWM pin to control motor speed
#define DIR_PIN_1 7        // Direction pin (direction 1)
#define DIR_PIN_2 8        // Direction pin (direction 2)

// ==================== GLOBAL VARIABLES ====================
// Duty cycle values for speed control (0-255)
const int SPEED_LOW = 150;  // Low motor speed
const int SPEED_HIGH = 250; // High motor speed

// ==================== INITIAL CONFIGURATION ====================
void setup() {
  // Set pins as outputs
  pinMode(PWM_PIN, OUTPUT);
  pinMode(DIR_PIN_1, OUTPUT);
  pinMode(DIR_PIN_2, OUTPUT);
}

// ==================== FUNCTION TO CONTROL THE MOTOR ====================
/**
 * Controls the motor by setting direction and speed.
 *
 * @param dir1 State of direction pin 1 (HIGH or LOW)
 * @param dir2 State of direction pin 2 (HIGH or LOW)
 * @param speed Motor speed (PWM value between 0 and 255)
 */
void controlMotor(bool dir1, bool dir2, int speed) {
  digitalWrite(DIR_PIN_1, dir1);  // Set direction on pin 1
  digitalWrite(DIR_PIN_2, dir2);  // Set direction on pin 2
  analogWrite(PWM_PIN, speed);    // Set speed using PWM
}

// ==================== MAIN LOOP ====================
void loop() {
  // Direction 1: Rotate in one direction at low speed
  controlMotor(HIGH, LOW, SPEED_LOW);
  delay(3000); // Maintain this state for 3 seconds

  // Direction 2: Rotate in the opposite direction at high speed
  controlMotor(LOW, HIGH, SPEED_HIGH);
  delay(3000); // Maintain this state for 3 seconds
}