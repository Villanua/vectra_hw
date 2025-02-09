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
#define M2_DIR_PIN_1 5   // Direction pin 1
#define M2_DIR_PIN_2 6   // Direction pin 2
#define M2_PWM_PIN 4     // PWM control pin
#define M2_ENCODER_PIN_A 7  // Encoder A pin
#define M2_ENCODER_PIN_B 15 // Encoder B pin

// ==================== PWM CONFIGURATION ====================
#define PWM_FREQ 5000       // PWM frequency (Hz)
#define PWM_RESOLUTION 10   // PWM resolution (bits)

// ==================== PCNT CONFIGURATION ====================
#define PCNT_H_LIM 32767    // High limit for pulse counter
#define PCNT_L_LIM -32767   // Low limit for pulse counter

// ==================== MOTOR STRUCTURE ====================
// Structure to hold motor parameters
struct Motor {
    int dirPin1;
    int dirPin2;
    int pwmPin;
    int pwmChannel;
    int encoderPinA;
    int encoderPinB;
    pcnt_unit_t pcntUnit;
    volatile int16_t encoderPosition;
    int16_t lastEncoderPosition;
    float motorSpeed;
};

// Motor instances
Motor motor1 = {M1_DIR_PIN_1, M1_DIR_PIN_2, M1_PWM_PIN, 0, M1_ENCODER_PIN_A, M1_ENCODER_PIN_B, PCNT_UNIT_0, 0, 0, 0};
Motor motor2 = {M2_DIR_PIN_1, M2_DIR_PIN_2, M2_PWM_PIN, 1, M2_ENCODER_PIN_A, M2_ENCODER_PIN_B, PCNT_UNIT_1, 0, 0, 0};

// ==================== FUNCTION TO CONFIGURE ENCODER ====================
/**
 * @brief Configures the pulse counter for encoder feedback
 * @param motor Reference to the motor structure
 */
void setupPulseCounter(Motor &motor) {
    pcnt_config_t pcntConfig = {};
    pcntConfig.pulse_gpio_num = motor.encoderPinA;
    pcntConfig.ctrl_gpio_num = motor.encoderPinB;
    pcntConfig.channel = PCNT_CHANNEL_0;
    pcntConfig.unit = motor.pcntUnit;
    pcntConfig.pos_mode = PCNT_COUNT_INC;
    pcntConfig.neg_mode = PCNT_COUNT_DEC;
    pcntConfig.lctrl_mode = PCNT_MODE_REVERSE;
    pcntConfig.hctrl_mode = PCNT_MODE_KEEP;
    pcntConfig.counter_h_lim = PCNT_H_LIM;
    pcntConfig.counter_l_lim = PCNT_L_LIM;

    pcnt_unit_config(&pcntConfig);
    pcnt_set_filter_value(motor.pcntUnit, 100);
    pcnt_filter_enable(motor.pcntUnit);
    pcnt_counter_pause(motor.pcntUnit);
    pcnt_counter_clear(motor.pcntUnit);
    pcnt_counter_resume(motor.pcntUnit);
}

// ==================== INITIAL SETUP ====================
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

// ==================== FUNCTION TO CONTROL MOTOR ====================
/**
 * @brief Controls motor direction and speed
 * @param motor Reference to the motor structure
 * @param dir1 State of direction pin 1
 * @param dir2 State of direction pin 2
 * @param speed PWM speed value (0-1023)
 */
void controlMotor(Motor &motor, bool dir1, bool dir2, int speed) {
    speed = constrain(speed, 0, 1023);
    digitalWrite(motor.dirPin1, dir1);
    digitalWrite(motor.dirPin2, dir2);
    ledcWrite(motor.pwmChannel, speed);
}

// ==================== FUNCTION TO MEASURE SPEED ====================
/**
 * @brief Calculates motor speed based on encoder readings
 * @param motor Reference to the motor structure
 */
void calculateSpeed(Motor &motor) {
    static unsigned long lastTime = 0;
    unsigned long currentTime = millis();

    if (currentTime - lastTime >= 100) { // Update every 100ms
        pcnt_get_counter_value(motor.pcntUnit, (int16_t*)&motor.encoderPosition);
        motor.motorSpeed = (motor.encoderPosition - motor.lastEncoderPosition) / ((currentTime - lastTime) / 1000.0);
        motor.lastEncoderPosition = motor.encoderPosition;
        lastTime = currentTime;
    }
}

// ==================== FUNCTION TO CONTROL MOTOR PROGRESSIVELY ====================
/**
 * @brief Gradually increases and decreases motor speed
 * @param motor Reference to the motor structure
 * @param targetSpeed Desired speed target
 */
void controlSpeedMotor(Motor &motor, int targetSpeed) {
    int pwmValue = 0;
    int maxPWM = 1023;
    int minPWM = 200;

    // Gradually accelerate
    while (abs(motor.motorSpeed) < targetSpeed) {
        pwmValue += 50;
        pwmValue = constrain(pwmValue, minPWM, maxPWM);
        controlMotor(motor, HIGH, LOW, pwmValue);
        calculateSpeed(motor);
        delay(500);
    }

    // Gradually decelerate
    while (abs(motor.motorSpeed) > 0) {
        pwmValue -= 50;
        pwmValue = constrain(pwmValue, minPWM, maxPWM);
        controlMotor(motor, HIGH, LOW, pwmValue);
        calculateSpeed(motor);
        delay(500);
    }
}

// ==================== MAIN LOOP ====================
void loop() {
    Serial.println("Gradually increasing speeds...");
    controlSpeedMotor(motor1, 600);
    controlSpeedMotor(motor2, 1000);
}
