#include "calstream.h"

calibration::calibration(UpdateFunction f, double inertia, double power = 2) {
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
    double _inertia = calibrated ? inertia : 0;
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
    raw_center = (counter * center + reading) / ((double) counter + 1);
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
stream::stream(UpdateFunction f, double calibration_inertia, double power = 2) {
    acquire_value = f;
    cal = calibration(acquire_value, calibration_inertia, power);
}

int stream::diff() {
    return rawValue - oldValue1;
}

double stream::relative() {
    return ((double) smoothedValue - cal.min_)/(cal.max_ - cal.min_);
}

// Implementation for stream update function
void stream::update() {
    rawValue = (*acquire_value)();
    oldValue7 = oldValue6;
    oldValue6 = oldValue5;
    oldValue5 = oldValue4;
    oldValue4 = oldValue3;
    oldValue3 = oldValue2;
    oldValue2 = oldValue1;
    oldValue1 = rawValue;
    smoothedValue = (rawValue + oldValue1 + oldValue2 + oldValue3 + oldValue4 + oldValue5 + oldValue6 + oldValue7) / 8;
}
// Implementation for stream zscore function
double stream::zscore(double kurtosis) {
    return ((double)smoothedValue - cal.center) / (cal.spread * kurtosis);
}

// Implementation for stream bell_curve function
double stream::bell_curve(double kurtosis) {
    double yval = exp(-1 * zscore(kurtosis) * zscore(kurtosis) / 2);
    return yval;
}
double stream::spread(double power = 2) {
    double center = (double) smoothedValue;
    double power_sum = pow(abs(center - rawValue), power)
                    + pow(abs(center - oldValue1), power) 
                    + pow(abs(center - oldValue2), power) 
                    + pow(abs(center - oldValue3), power) 
                    + pow(abs(center - oldValue4), power) 
                    + pow(abs(center - oldValue5), power) 
                    + pow(abs(center - oldValue6), power) 
                    + pow(abs(center - oldValue7), power);
    return pow(power_sum / 8, 1 / power);
}   