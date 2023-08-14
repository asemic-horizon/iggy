#include "calstream.h"

// Implementation for calibration update function
void calibration::update(float new_center, float new_spread, float new_min, float new_max, float inertia) {
    center = (inertia * center + new_center) / (1 + inertia);
    spread = (inertia * spread + new_spread) / (1 + inertia);
    min_ = (inertia * min_ + new_min) / (1 + inertia);
    max_ = (inertia * max_ + new_max) / (1 + inertia);
}

// Implementation for stream constructor
stream::stream(UpdateFunction f, float calibration_inertia, unsigned long duration) {
    acquire_value = f;
    this->calibration_inertia = calibration_inertia;
    calibrate(duration);
}

int stream::diff() {
    return rawValue - oldValue1;
}

// Implementation for stream update function
void stream::update() {
    int newvalue = (*acquire_value)();
    oldValue2 = oldValue1;
    oldValue1 = rawValue;
    rawValue = newvalue;
    smoothedValue = (oldValue1 + oldValue2 + newvalue) / 3;
}

// Implementation for stream calibrate function
void stream::calibrate(unsigned long duration, double power) {
    double center = 0;
    double spread = 0;
    double reading;
    double counter = 1;
    unsigned long t0 = millis();
    int min_ = 1023;
    int max_ = 0;
    while (millis() < t0 + duration) {
        update();
        reading = (double)rawValue;
        min_ = min(min_, reading);
        max_ = max(max_, reading);
        spread = (counter * spread + pow(abs(reading - center), power)) / (counter + 1);
        center = (counter * center + reading) / (counter + 1);
        delay(2);
    }
    spread = pow(spread, 1 / power);
    float inertia = calibrated ? calibration_inertia : 0;
    cal.update(center, spread, min_, max_, inertia);
    calibrated = true;
}

// Implementation for stream zscore function
float stream::zscore(float kurtosis) {
    return ((float)smoothedValue - cal.center) / (cal.spread * kurtosis);
}

// Implementation for stream bell_curve function
float stream::bell_curve(float kurtosis) {
    float yval = exp(-1 * zscore(kurtosis) * zscore(kurtosis) / 2);
    return yval;
}
