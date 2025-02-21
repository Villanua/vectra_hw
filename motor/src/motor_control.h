#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <Arduino.h>
#include "driver/pcnt.h"

// ==================== PIN CONFIGURATION ====================
// Motor 1
#define M1_DIR_PIN_1 16  // Direction pin 1
#define M1_DIR_PIN_2 17  // Direction pin 2
#define M1_PWM_PIN 18    // PWM control pin
#define M1_ENCODER_PIN_A 8  // Encoder A pin
#define M1_ENCODER_PIN_B 3  // Encoder B pin

// Motor 2
#define M2_DIR_PIN_1 6   // Direction pin 1
#define M2_DIR_PIN_2 5   // Direction pin 2
#define M2_PWM_PIN 4     // PWM control pin
#define M2_ENCODER_PIN_A 7  // Encoder A pin
#define M2_ENCODER_PIN_B 15 // Encoder B pin

// ==================== PWM CONFIGURATION ====================
#define PWM_FREQ 5000       // PWM frequency (Hz)
#define PWM_RESOLUTION 10   // PWM resolution (bits)

// ==================== PCNT CONFIGURATION ====================
#define PCNT_H_LIM 32767    // High limit for pulse counter
#define PCNT_L_LIM -32767   // Low limit for pulse counter

#define FREQUENCY 20      // Frequency of control loop (Hz)

// ==================== PID STRUCTURE ====================
struct PID {
    float kp;
    float ki;
    float kd;
    float setpoint;
    float accumulatedError;
    float previousError;
};

// ==================== MOTOR STRUCTURE ====================
// Structure to hold motor parameters
struct Motor {
    int dirPin1;
    int dirPin2;
    int pwmPin;
    int pwmChannel;
    int minPWM;
    int maxPWM;
    int encoderPinA;
    int encoderPinB;
    pcnt_unit_t pcntUnit;
    volatile int16_t encoderPosition;
    int16_t lastEncoderPosition;
    float motorSpeed;
    unsigned long lastMeasurementTime;
    PID pid;
};

// Function declarations
void setupPulseCounter(Motor &motor);
float controlPID(PID &pid, float currentSpeed);
void moveMotor(Motor &motor, bool dir1, bool dir2, int speed);
void measureSpeed(Motor &motor);
void controlSpeedMotor(Motor &motor, int targetSpeed);

#endif // MOTOR_CONTROL_H