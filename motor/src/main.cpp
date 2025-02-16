#include <Arduino.h>
#include "motor_control.h"

// Motor instances
Motor motor1 = {
    M1_DIR_PIN_1,      // Direction pin 1
    M1_DIR_PIN_2,      // Direction pin 2
    M1_PWM_PIN,        // PWM control pin
    0,                 // PWM channel
    0,                 // Minimum PWM value
    1023,               // Maximum PWM value
    M1_ENCODER_PIN_A,  // Encoder A pin
    M1_ENCODER_PIN_B,  // Encoder B pin
    PCNT_UNIT_0,       // Pulse counter unit
    0,                 // encoderPosition
    0,                 // lastEncoderPosition
    0.0,               // motorSpeed
    0,                 // lastMeasurementTime
    {                  // PID instance
        1.0,           // kp
        0.1,           // ki
        0.01,          // kd
        0,             // setpoint
        0,             // accumulatedError
        0              // previousError
    }
};

Motor motor2 = {
    M2_DIR_PIN_1,      // Direction pin 1
    M2_DIR_PIN_2,      // Direction pin 2
    M2_PWM_PIN,        // PWM control pin
    1,                 // PWM channel
    0,                 // Minimum PWM value
    800,               // Maximum PWM value
    M2_ENCODER_PIN_A,  // Encoder A pin
    M2_ENCODER_PIN_B,  // Encoder B pin
    PCNT_UNIT_1,       // Pulse counter unit
    0,                 // encoderPosition
    0,                 // lastEncoderPosition
    0.0,               // motorSpeed
    0,                 // lastMeasurementTime
    {                  // PID instance
        1.0,           // kp
        0.1,           // ki
        0.01,          // kd
        0,             // setpoint
        0,             // accumulatedError
        0              // previousError
    }
};

void setup() {
    Serial.begin(115200);

    // Set motor direction pins as outputs
    pinMode(motor1.dirPin1, OUTPUT);
    pinMode(motor1.dirPin2, OUTPUT);
    pinMode(motor2.dirPin1, OUTPUT);
    pinMode(motor2.dirPin2, OUTPUT);

    // Configure PWM channels
    ledcSetup(motor1.pwmChannel, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(motor1.pwmPin, motor1.pwmChannel);
    ledcSetup(motor2.pwmChannel, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(motor2.pwmPin, motor2.pwmChannel);

    // Initialize encoders
    setupPulseCounter(motor1);
    setupPulseCounter(motor2);

    Serial.println("Motor control initialized with encoder feedback");
}

// ==================== MAIN LOOP ====================
void loop() {
    controlSpeedMotor(motor1, 500);
    // controlSpeedMotor(motor2, 1000);
    delay(100);
}
