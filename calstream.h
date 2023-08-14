#ifndef CALSTREAM_H
#define CALSTREAM_H

#include <Arduino.h>

// Define the constant SECONDS
#define SECONDS * 1000

// Update function pointer
typedef int (*UpdateFunction)();

// Struct for calibration
struct calibration {
    float center;
    float spread;
    float min_;
    float max_;

    // Constructor
    calibration() : center(0.0), spread(1.0), min_(0.0), max_(1023.0) {}

    void update(float new_center, float new_spread, float new_min, float new_max, float inertia);
};

// Class for stream
class stream {
public:
    stream(UpdateFunction f, float calibration_inertia = 3, unsigned long duration = 5 SECONDS);
    calibration cal;
    void update();
    int diff();
    void calibrate(unsigned long duration = 5 SECONDS, double power = 2);
    float zscore(float kurtosis = 1);
    float bell_curve(float kurtosis = 1);
    int smoothedValue;

private:
    int rawValue;
    int oldValue2;
    int oldValue1;
    float calibration_inertia;
    bool calibrated;
    UpdateFunction acquire_value;
};

#endif
