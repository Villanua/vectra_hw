#include "motor_control.h"

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

float calculatePID(PID &pid, float currentSpeed) {
    float error = pid.setpoint - currentSpeed;
    pid.accumulatedError += error;
    float derivative = error - pid.previousError;
    pid.previousError = error;

    // Debugging prints
    Serial.print("Reference Speed: ");
    Serial.println(pid.setpoint);
    Serial.print("Current Speed: ");
    Serial.println(currentSpeed);
    Serial.print("Error: ");
    Serial.println(error);
    Serial.print("Accumulated Error: ");
    Serial.println(pid.accumulatedError);
    Serial.print("Derivative: ");
    Serial.println(derivative);
    Serial.print("PID output: ");
    Serial.println(pid.kp * error + pid.ki * pid.accumulatedError + pid.kd * derivative);

    return pid.kp * error + pid.ki * pid.accumulatedError + pid.kd * derivative;
}

void controlMotor(Motor &motor, bool dir1, bool dir2, int speed) {
    digitalWrite(motor.dirPin1, dir1);
    digitalWrite(motor.dirPin2, dir2);
    Serial.print("Final speed: ");
    Serial.println(speed);
    ledcWrite(motor.pwmChannel, speed);
}

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

void controlSpeedMotor(Motor &motor, int targetSpeed) {
    motor.pid.setpoint = targetSpeed;

    calculateSpeed(motor);
    float pidOutput = calculatePID(motor.pid, motor.motorSpeed);
    pidOutput = constrain(abs(pidOutput), motor.minPWM, motor.maxPWM);

    if (pidOutput < 0) {
        controlMotor(motor, HIGH, LOW, pidOutput); // Forward direction
    } else {
        controlMotor(motor, LOW, HIGH, pidOutput); // Reverse direction
    }

    delay(100);
}
