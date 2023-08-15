#include "calstream.h"

calibration::calibration(UpdateFunction f, float inertia, float power = 2) {
    acquire_value = f;
    this->inertia = inertia;
    this->power = power;

    min_ = 1023; raw_min_ = 1023;
    max_= 0; raw_max_ = 0;
    raw_center = 0;
    raw_spread = 0;
    center = 0;
    spread = 0;
}
void calibration::inertial_update() {
    float _inertia = calibrated ? inertia : 0;
    center = (_inertia * center + raw_center) / (1 + _inertia);
    spread = (_inertia * spread + raw_spread) / (1 + _inertia);
    min_ = (_inertia * min_ + raw_min_) / (1 + _inertia);
    max_ = (_inertia * max_ + raw_max_) / (1 + _inertia);
}
void calibration::begin(){
    start_time = millis();
    counter = 1;
}
void calibration::step() {
    double reading = (double)(*acquire_value)();
    double spread_p = (counter * spread_p + pow(abs(reading - center), power)) / ((double) counter + 1);
    raw_center = (counter * center + reading) / ((float) counter + 1);
    raw_spread = pow(spread_p,1/power);
    counter++;
    delay(2);
    }
void calibration::end() {
    inertial_update();
    spread = min(spread,1);
    calibrated = true;
}
// Implementation for stream constructor
stream::stream(UpdateFunction f, float calibration_inertia, float power = 2) {
    acquire_value = f;
    cal = calibration(acquire_value, calibration_inertia, power);
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
// Implementation for stream zscore function
float stream::zscore(float kurtosis) {
    return ((float)smoothedValue - cal.center) / (cal.spread * kurtosis);
}

// Implementation for stream bell_curve function
float stream::bell_curve(float kurtosis) {
    float yval = exp(-1 * zscore(kurtosis) * zscore(kurtosis) / 2);
    return yval;
}
