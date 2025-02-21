#include "motor_control.h"

void IRAM_ATTR pcntOverflowHandler(void *arg) {
    Motor *motor = (Motor *)arg;
    uint32_t status = 0;
    pcnt_get_event_status(motor->pcntUnit, &status);

    if (status & PCNT_EVT_H_LIM) {
        motor->encoderPosition += 32767;
        motor->lastEncoderPosition -= 32767;
    } else if (status & PCNT_EVT_L_LIM) {
        motor->encoderPosition -= 32767;
        motor->lastEncoderPosition += 32767;
    }
}

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

    // Enable PCNT interrupt
    pcnt_event_enable(motor.pcntUnit, PCNT_EVT_H_LIM);
    pcnt_event_enable(motor.pcntUnit, PCNT_EVT_L_LIM);
    pcnt_isr_service_install(0);
    pcnt_isr_handler_add(motor.pcntUnit, pcntOverflowHandler, (void *)&motor);
    pcnt_intr_enable(motor.pcntUnit);
}

float controlPID(PID &pid, float feedback) {
    int controlSignal = 0;
    float error = pid.setpoint - feedback;
    pid.accumulatedError += error;
    float derivative = error - pid.previousError;
    pid.previousError = error;

    controlSignal = pid.kp * error + pid.ki * pid.accumulatedError + pid.kd * derivative;

    // Debugging prints
    Serial.print("Current Time: ");
    Serial.println(millis());
    Serial.print("Reference: ");
    Serial.println(pid.setpoint);
    Serial.print("Feedback: ");
    Serial.println(feedback);
    Serial.print("Current Error: ");
    Serial.println(error);
    Serial.print("Accumulated Error: ");
    Serial.println(pid.accumulatedError);
    Serial.print("Derivative Error: ");
    Serial.println(derivative);
    Serial.print("Control Signal: ");
    Serial.println(controlSignal);

    return controlSignal;
}

void moveMotor(Motor &motor, bool dir1, bool dir2, int speed) {
    digitalWrite(motor.dirPin1, dir1);
    digitalWrite(motor.dirPin2, dir2);
    Serial.print("Final Control Signal: ");
    Serial.println(speed);
    ledcWrite(motor.pwmChannel, speed);
}

// Measure the speed of the motor
void measureSpeed(Motor &motor) {
    unsigned long currentTime = millis();
    int16_t newPosition;
    pcnt_get_counter_value(motor.pcntUnit, &newPosition);

    // Handle overflow
    if (newPosition < motor.lastEncoderPosition && motor.lastEncoderPosition + newPosition > 32767) {
        motor.encoderPosition += (newPosition + 32767 - motor.lastEncoderPosition);
    } else if (newPosition > motor.lastEncoderPosition && newPosition + motor.lastEncoderPosition < -32767) {
        motor.encoderPosition -= (motor.lastEncoderPosition + 32767 - newPosition);
    } else {
        motor.encoderPosition += (newPosition - motor.lastEncoderPosition);
    }

    motor.motorSpeed = (motor.encoderPosition - motor.lastEncoderPosition) / ((currentTime - motor.lastMeasurementTime) / 1000.0);
    motor.lastEncoderPosition = newPosition;
    motor.lastMeasurementTime = currentTime;
}


void controlSpeedMotor(Motor &motor, int targetSpeed) {
    motor.pid.setpoint = targetSpeed;

    measureSpeed(motor);
    float controlSignal = controlPID(motor.pid, motor.motorSpeed);
    controlSignal = constrain(abs(controlSignal), motor.minPWM, motor.maxPWM);

    if (targetSpeed < 0) {
        moveMotor(motor, HIGH, LOW, controlSignal); // Forward direction
    } else {
        moveMotor(motor, LOW, HIGH, controlSignal); // Reverse direction
    }
}