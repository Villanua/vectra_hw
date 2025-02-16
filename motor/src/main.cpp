#include <Arduino.h>
#include "motor_control.h"

// Motor instances
Motor motor1 = {
    M1_DIR_PIN_1,      // Direction pin 1
    M1_DIR_PIN_2,      // Direction pin 2
    M1_PWM_PIN,        // PWM control pin
    0,                 // PWM channel
    M1_ENCODER_PIN_A,  // Encoder A pin
    M1_ENCODER_PIN_B,  // Encoder B pin
    PCNT_UNIT_0,       // Pulse counter unit
    {                  // PID instance
        1.0,        // kp
        0.1,          // ki
        0.01,         // kd
        0,            // setpoint
        0,            // accumulatedError
        0             // previousError
    },
    0,                 // Minimum PWM value
    800                // Maximum PWM value
};

Motor motor2 = {
    M2_DIR_PIN_1,      // Direction pin 1
    M2_DIR_PIN_2,      // Direction pin 2
    M2_PWM_PIN,        // PWM control pin
    1,                 // PWM channel
    M2_ENCODER_PIN_A,  // Encoder A pin
    M2_ENCODER_PIN_B,  // Encoder B pin
    PCNT_UNIT_1,       // Pulse counter unit
    {                  // PID instance
        1.0,        // kp
        0.1,          // ki
        0.01,         // kd
        0,            // setpoint
        0,            // accumulatedError
        0             // previousError
    },
    0,                 // Minimum PWM value
    1023                // Maximum PWM value
};

void setup() {
    Serial.begin(115200);
    delay(1000); // Agregar un retraso de 1 segundo

    Serial.println("Motor control initialized with encoder feedback");

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

    Serial.println("Setup complete");
}

void loop() {
    controlSpeedMotor(motor1, 200);
    controlSpeedMotor(motor2, 500);
}