#ifndef CALSTREAM_H
#define CALSTREAM_H

#include <Arduino.h>


typedef int (*UpdateFunction)(); // a "sensor" (a function that reads values)

class calibration {
public:
    calibration(UpdateFunction f, double inertia = 3, double power=2);
    void inertial_update();
    void begin();
    void step();
    void end();

    double center;
    double spread;
    double min_;
    double max_;
private:
    double power; // the spread is the average Lp norm of (value-center)
    unsigned long counter;
    double raw_center, raw_spread, raw_min_, raw_max_;
    unsigned long start_time;
    bool calibrated;
    double inertia;
    UpdateFunction acquire_value;

};

class stream {
public:
    stream(UpdateFunction f, double calibration_inertia = 3, double power = 2);
    void update();
    int diff();
    double relative();
    double zscore(double kurtosis = 1); // kurtosis is multiplied by the calibrated spread
    double bell_curve(double kurtosis = 1); // to obtain higher values at the tails.
    double spread(double power = 2); // running spread stat
    int smoothedValue;
    calibration cal = calibration(acquire_value, (double) 3.0, (double) 2.0);
private:
    int rawValue;
    int oldValue7;
    int oldValue6;
    int oldValue5;
    int oldValue4;
    int oldValue3;
    int oldValue2;
    int oldValue1;
    UpdateFunction acquire_value;
};

#endif
