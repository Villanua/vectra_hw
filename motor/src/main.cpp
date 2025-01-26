#include <Arduino.h>

// ==================== PIN CONFIGURATION ====================
#define M1_DIR_PIN_1 16        // Motor 1 Direction pin (direction 1)
#define M1_DIR_PIN_2 17        // Motor 1 Direction pin (direction 2)
#define M1_PWM_PIN 18          // Motor 1 PWM pin to control speed

#define M2_DIR_PIN_1 4         // Motor 2 Direction pin (direction 1)
#define M2_DIR_PIN_2 5         // Motor 2 Direction pin (direction 2)
#define M2_PWM_PIN 6           // Motor 2 PWM pin to control speed

// ==================== PWM CONFIGURATION ====================
#define M1_PWM_CHANNEL 0       // LEDC channel for Motor 1
#define M2_PWM_CHANNEL 1       // LEDC channel for Motor 2
#define PWM_FREQ 5000          // Frequency in Hz
#define PWM_RESOLUTION 10      // 10-bit resolution (0-1023)

// ==================== GLOBAL VARIABLES ====================
const int SPEED_LOW = 700;  // Low motor speed (PWM duty cycle)
const int SPEED_HIGH = 1023; // High motor speed (PWM duty cycle)

// ==================== INITIAL CONFIGURATION ====================
void setup() {
  // Configure direction pins as outputs for both motors
  pinMode(M1_DIR_PIN_1, OUTPUT);
  pinMode(M1_DIR_PIN_2, OUTPUT);
  pinMode(M2_DIR_PIN_1, OUTPUT);
  pinMode(M2_DIR_PIN_2, OUTPUT);

  // Configure the PWM pins using LEDC for both motors
  ledcSetup(M1_PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(M1_PWM_PIN, M1_PWM_CHANNEL);

  ledcSetup(M2_PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(M2_PWM_PIN, M2_PWM_CHANNEL);

  Serial.begin(115200);
  Serial.println("Dual motor control initialized");
}

// ==================== FUNCTION TO CONTROL THE MOTORS ====================
/**
 * Controls a motor by setting its direction and speed.
 *
 * @param dir1 State of direction pin 1 (HIGH or LOW)
 * @param dir2 State of direction pin 2 (HIGH or LOW)
 * @param speed Motor speed (PWM duty cycle between 0 and 1023)
 * @param pwm_channel PWM channel assigned to the motor
 */
void controlMotor(int dirPin1, int dirPin2, int pwm_channel, bool dir1, bool dir2, int speed) {
  speed = constrain(speed, 0, 1023); // Limit the speed value
  digitalWrite(dirPin1, dir1);
  digitalWrite(dirPin2, dir2);
  ledcWrite(pwm_channel, speed);
}

// ==================== MAIN LOOP ====================
void loop() {
  // Control both motors: rotate in one direction at low speed
  Serial.println("Both motors rotating in direction 1 at low speed");
  controlMotor(M1_DIR_PIN_1, M1_DIR_PIN_2, M1_PWM_CHANNEL, HIGH, LOW, SPEED_LOW);
  controlMotor(M2_DIR_PIN_1, M2_DIR_PIN_2, M2_PWM_CHANNEL, HIGH, LOW, SPEED_LOW);
  delay(3000); // Maintain this state for 3 seconds

  // Control both motors: rotate in the opposite direction at high speed
  Serial.println("Both motors rotating in direction 2 at high speed");
  controlMotor(M1_DIR_PIN_1, M1_DIR_PIN_2, M1_PWM_CHANNEL, LOW, HIGH, SPEED_HIGH);
  controlMotor(M2_DIR_PIN_1, M2_DIR_PIN_2, M2_PWM_CHANNEL, LOW, HIGH, SPEED_HIGH);
  delay(3000); // Maintain this state for 3 seconds
}