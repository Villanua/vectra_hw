#include <Arduino.h>

// ==================== PIN CONFIGURATION ====================
#define PWM_PIN 16          // PWM pin to control motor speed
#define DIR_PIN_1 17        // Direction pin (direction 1)
#define DIR_PIN_2 18        // Direction pin (direction 2)

// ==================== PWM CONFIGURATION ====================
#define PWM_CHANNEL 0       // LEDC channel (0-15)
#define PWM_FREQ 5000       // Frequency in Hz
#define PWM_RESOLUTION 10   // Resolución de 10 bits (0-1023)

// ==================== GLOBAL VARIABLES ====================
const int SPEED = 100;  // Low motor speed (PWM duty cycle)8
// ==================== INITIAL CONFIGURATION ====================
void setup() {
  // Configure direction pins as outputs
  pinMode(DIR_PIN_1, OUTPUT);
  pinMode(DIR_PIN_2, OUTPUT);

  // Configure the PWM pin using LEDC
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(PWM_PIN, PWM_CHANNEL);

  Serial.begin(115200);
  Serial.println("Motor control initialized");
}

// ==================== FUNCTION TO CONTROL THE MOTOR ====================
/**
 * Controls the motor by setting direction and speed.
 *
 * @param dir1 State of direction pin 1 (HIGH or LOW)
 * @param dir2 State of direction pin 2 (HIGH or LOW)
 * @param speed Motor speed (PWM duty cycle between 0 and 255)
 */
void controlMotor(bool dir1, bool dir2, int speed) {
  speed = constrain(speed, 0, 1023); // Limita el valor de speed
  digitalWrite(DIR_PIN_1, dir1);  
  digitalWrite(DIR_PIN_2, dir2);
  ledcWrite(PWM_CHANNEL, speed);
}

// ==================== MAIN LOOP ====================
void loop() {
  // Direction 1: Rotate in one direction at low speed
  Serial.println("Rotating in direction 1 at low speed");
  controlMotor(HIGH, LOW, SPEED);
  delay(3000); // Maintain this state for 3 seconds

  // Direction 2: Rotate in the opposite direction at high speed
  Serial.println("Rotating in direction 2 at high speed");
  controlMotor(LOW, HIGH, SPEED);
  delay(3000); // Maintain this state for 3 seconds
}
