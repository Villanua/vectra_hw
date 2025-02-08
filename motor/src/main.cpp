#include <Arduino.h>
#include "driver/pcnt.h"

// ==================== PIN CONFIGURATION ====================
#define M1_DIR_PIN_1 16        // Motor 1 Direction pin (direction 1)
#define M1_DIR_PIN_2 17        // Motor 1 Direction pin (direction 2)
#define M1_PWM_PIN 18          // Motor 1 PWM pin to control speed

#define M2_PWM_PIN 4           // Motor 2 PWM pin to control speed
#define M2_DIR_PIN_1 5         // Motor 2 Direction pin (direction 1)
#define M2_DIR_PIN_2 6         // Motor 2 Direction pin (direction 2)

// ==================== PWM CONFIGURATION ====================
#define M1_PWM_CHANNEL 0       // LEDC channel for Motor 1
#define M2_PWM_CHANNEL 1       // LEDC channel for Motor 2
#define PWM_FREQ 5000          // Frequency in Hz
#define PWM_RESOLUTION 10      // 10-bit resolution (0-1023)

// ==================== ENCODER CONFIGURATION ====================
#define M2_ENCODER_PIN_A 7     // Encoder signal A (Pulse Counter input)
#define M2_ENCODER_PIN_B 15    // Encoder signal B (Direction control)

// ==================== PCNT CONFIGURATION ====================
#define PCNT_UNIT PCNT_UNIT_0  // Pulse counter unit
#define PCNT_H_LIM 32767       // Max count limit
#define PCNT_L_LIM -32767      // Min count limit

volatile int16_t encoderPosition = 0;
int16_t lastEncoderPosition = 0;
float motorSpeed = 0;          // Motor speed in pulses per second (PPS)

// ==================== SETUP PULSE COUNTER ====================
void setupPulseCounter() {
    pcnt_config_t pcntConfig = {};
    pcntConfig.pulse_gpio_num = M2_ENCODER_PIN_A;  // Encoder pulse pin
    pcntConfig.ctrl_gpio_num = M2_ENCODER_PIN_B;   // Encoder direction pin
    pcntConfig.channel = PCNT_CHANNEL_0;
    pcntConfig.unit = PCNT_UNIT;
    pcntConfig.pos_mode = PCNT_COUNT_INC; // Increment count if ctrl_gpio is HIGH
    pcntConfig.neg_mode = PCNT_COUNT_DEC; // Decrement count if ctrl_gpio is LOW
    pcntConfig.lctrl_mode = PCNT_MODE_REVERSE; // Reverse count based on direction pin
    pcntConfig.hctrl_mode = PCNT_MODE_KEEP;    // Keep count when HIGH
    pcntConfig.counter_h_lim = PCNT_H_LIM;
    pcntConfig.counter_l_lim = PCNT_L_LIM;

    pcnt_unit_config(&pcntConfig);

    // Set filter to remove noise
    pcnt_set_filter_value(PCNT_UNIT, 100);
    pcnt_filter_enable(PCNT_UNIT);

    // Initialize counter
    pcnt_counter_pause(PCNT_UNIT);
    pcnt_counter_clear(PCNT_UNIT);
    pcnt_counter_resume(PCNT_UNIT);
}

// ==================== INITIAL CONFIGURATION ====================
void setup() {
    Serial.begin(115200);

    // Configure motor direction pins as output
    pinMode(M1_DIR_PIN_1, OUTPUT);
    pinMode(M1_DIR_PIN_2, OUTPUT);
    pinMode(M2_DIR_PIN_1, OUTPUT);
    pinMode(M2_DIR_PIN_2, OUTPUT);

    // Configure PWM for motors
    ledcSetup(M1_PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(M1_PWM_PIN, M1_PWM_CHANNEL);

    ledcSetup(M2_PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(M2_PWM_PIN, M2_PWM_CHANNEL);

    // Configure the encoder pulse counter
    setupPulseCounter();

    Serial.println("Motor control initialized with speed feedback from encoder");
}

// ==================== FUNCTION TO CONTROL THE MOTORS ====================
void controlMotor(int dirPin1, int dirPin2, int pwm_channel, bool dir1, bool dir2, int speed) {
    speed = constrain(speed, 0, 1023); // Limit speed value
    digitalWrite(dirPin1, dir1);
    digitalWrite(dirPin2, dir2);
    ledcWrite(pwm_channel, speed);
}

// ==================== FUNCTION TO MEASURE SPEED ====================
void calculateSpeed() {
    static unsigned long lastTime = 0;
    unsigned long currentTime = millis();

    if (currentTime - lastTime >= 100) {  // Update speed every 100ms
        pcnt_get_counter_value(PCNT_UNIT_0, (int16_t*)&encoderPosition);
        motorSpeed = (encoderPosition - lastEncoderPosition) / ((currentTime - lastTime) / 1000.0); // PPS
        lastEncoderPosition = encoderPosition;
        lastTime = currentTime;
    }
}

// ==================== FUNCTION TO CONTROL SPEED PROGRESSIVELY ====================
void controlSpeed(int targetSpeed) {
    int pwmValue = 0;
    int maxPWM = 1023;
    int minPWM = 200;

    while (abs(motorSpeed) < targetSpeed) {
        pwmValue += 50;  // Increase PWM gradually
        pwmValue = constrain(pwmValue, minPWM, maxPWM);
        
        controlMotor(M1_DIR_PIN_1, M1_DIR_PIN_2, M1_PWM_CHANNEL, HIGH, LOW, pwmValue);
        controlMotor(M2_DIR_PIN_1, M2_DIR_PIN_2, M2_PWM_CHANNEL, HIGH, LOW, pwmValue);

        calculateSpeed();
        Serial.print("Current Speed (PPS): ");
        Serial.println(motorSpeed);

        delay(500);
    }

    while (abs(motorSpeed) > 0) {
        pwmValue -= 50;  // Increase PWM gradually
        pwmValue = constrain(pwmValue, minPWM, maxPWM);
        
        controlMotor(M1_DIR_PIN_1, M1_DIR_PIN_2, M1_PWM_CHANNEL, HIGH, LOW, pwmValue);
        controlMotor(M2_DIR_PIN_1, M2_DIR_PIN_2, M2_PWM_CHANNEL, HIGH, LOW, pwmValue);

        calculateSpeed();
        Serial.print("Current Speed (PPS): ");
        Serial.println(motorSpeed);

        delay(500);
    }
}

// ==================== MAIN LOOP ====================
void loop() {
    Serial.println("Gradually increasing speed to 500 PPS...");
    controlSpeed(1000);
}
