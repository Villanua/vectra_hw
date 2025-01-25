#include <Arduino.h>

// ==================== PIN CONFIGURATION ====================
// Motor 1
#define PWM_PIN_1 16        // PWM pin for motor 1 speed
#define DIR_PIN_1 17        // Direction pin for motor 1 (direction 1)
#define DIR_PIN_2 18        // Direction pin for motor 1 (direction 2)

// Motor 2
#define PWM_PIN_2 37        // PWM pin for motor 2 speed
#define DIR_PIN_3 38        // Direction pin for motor 2 (direction 1)
#define DIR_PIN_4 39        // Direction pin for motor 2 (direction 2)

// ==================== PWM CONFIGURATION ====================
#define PWM_CHANNEL_1 0     // LEDC channel for motor 1 (0-15)
#define PWM_CHANNEL_2 1     // LEDC channel for motor 2 (0-15)
#define PWM_FREQ 5000       // Frequency in Hz
#define PWM_RESOLUTION 10   // PWM resolution (10 bits = 0-1023)

// ==================== GLOBAL VARIABLES ====================
const int SPEED_LOW = 150;  // Low motor speed (PWM duty cycle)
const int SPEED_HIGH = 1023; // High motor speed (PWM duty cycle)

// ==================== INITIAL CONFIGURATION ====================
void setup() {
  // Configure direction pins as outputs
  pinMode(DIR_PIN_1, OUTPUT);
  pinMode(DIR_PIN_2, OUTPUT);
  pinMode(DIR_PIN_3, OUTPUT);
  pinMode(DIR_PIN_4, OUTPUT);

  // Configure PWM channels for both motors
  ledcSetup(PWM_CHANNEL_1, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(PWM_PIN_1, PWM_CHANNEL_1);

  ledcSetup(PWM_CHANNEL_2, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(PWM_PIN_2, PWM_CHANNEL_2);

  Serial.begin(115200);
  Serial.println("Dual motor control initialized");
}

// ==================== FUNCTION TO CONTROL A MOTOR ====================
/**
 * Controls a motor by setting direction and speed.
 *
 * @param dirPin1 State of direction pin 1 (HIGH or LOW)
 * @param dirPin2 State of direction pin 2 (HIGH or LOW)
 * @param pwmChannel LEDC PWM channel for the motor
 * @param speed Motor speed (PWM duty cycle between 0 and 1023)
 */
void controlMotor(int dirPin1, int dirPin2, int pwmChannel, bool dir1, bool dir2, int speed) {
  digitalWrite(dirPin1, dir1);      // Set direction on pin 1
  digitalWrite(dirPin2, dir2);      // Set direction on pin 2
  ledcWrite(pwmChannel, speed);    // Set PWM speed
}

// ==================== MAIN LOOP ====================
void loop() {
  // Motor 1: Rotate in one direction at low speed
  Serial.println("Motor 1: Rotating in direction 1 at low speed");
  // controlMotor(DIR_PIN_1, DIR_PIN_2, PWM_CHANNEL_1, HIGH, LOW, SPEED_LOW);
  delay(3000); // Maintain this state for 3 seconds

  // Motor 1: Rotate in the opposite direction at high speed
  Serial.println("Motor 1: Rotating in direction 2 at high speed");
  // controlMotor(DIR_PIN_1, DIR_PIN_2, PWM_CHANNEL_1, LOW, HIGH, SPEED_HIGH);
  delay(3000); // Maintain this state for 3 seconds

  // Motor 2: Rotate in one direction at low speed
  Serial.println("Motor 2: Rotating in direction 1 at low speed");
  controlMotor(DIR_PIN_3, DIR_PIN_4, PWM_CHANNEL_2, HIGH, LOW, SPEED_HIGH);
  delay(3000); // Maintain this state for 3 seconds

  // Motor 2: Rotate in the opposite direction at high speed
  Serial.println("Motor 2: Rotating in direction 2 at high speed");
  controlMotor(DIR_PIN_3, DIR_PIN_4, PWM_CHANNEL_2, LOW, HIGH, SPEED_HIGH);
  delay(3000); // Maintain this state for 3 seconds
}