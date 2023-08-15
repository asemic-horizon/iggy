#ifndef CALSTREAM_H
#define CALSTREAM_H

#include <Arduino.h>

#define SECONDS * 1000

typedef int (*UpdateFunction)(); // a "sensor" (a function that reads values)

class calibration {
public:
    calibration(UpdateFunction f, float inertia = 3, float power=2);
    void inertial_update();
    void begin();
    void step();
    void end();

    float center;
    float spread;
    float min_;
    float max_;
private:
    float power; // the spread is the average Lp norm of (value-center)
    unsigned long counter;
    float raw_center, raw_spread, raw_min_, raw_max_;
    unsigned long start_time;
    bool calibrated;
    float inertia;
    UpdateFunction acquire_value;

};

class stream {
public:
    stream(UpdateFunction f, float calibration_inertia = 3, float power = 2);
    void update();
    int diff();
    float zscore(float kurtosis = 1); // kurtosis is multiplied by the calibrated spread
    float bell_curve(float kurtosis = 1); // to obtain higher values at the tails.
    int smoothedValue;
    calibration cal = calibration(acquire_value, (float) 3.0, (float) 2.0);
private:
    int rawValue;
    int oldValue2;
    int oldValue1;
    UpdateFunction acquire_value;
};

#endif
