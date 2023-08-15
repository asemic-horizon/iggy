#ifndef HW_LAYOUT
#define HW_LAYOUT
#include "calstream.h"
#include "leds.h"
#define SECONDS * 1000

// Our literal hardware layout, i.e.
// which pins correspond to which peripherals
const int BLACK_LED_PIN =3;
const int RED_LED_PIN = 9;
const int X_LED_PIN = 5;
const int Y_LED_PIN = 6;
const int Z_LED_PIN = 10;
const int SERVO_PIN = 11;

const int HEAT_SENSOR_PIN = A0;
const int FRONT_LIGHT_SENSOR_PIN = A1;
const int SIDE_LIGHT_SENSOR_PIN = A3;
const int SIDE_HEAT_SENSOR_PIN = A4;


void pin_setup() {
  // set sensors as inputs, actuators as outputs
  pinMode(FRONT_LIGHT_SENSOR_PIN, INPUT);
  pinMode(SIDE_LIGHT_SENSOR_PIN, INPUT);
  pinMode(HEAT_SENSOR_PIN, INPUT);
  pinMode(SIDE_HEAT_SENSOR_PIN, INPUT);

  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BLACK_LED_PIN, OUTPUT);
  pinMode(SERVO_PIN, OUTPUT);
}

// initialize the leds
const int MAX_BLACK = 255;
const int MAX_RED = 64;
const int MAX_RGB = 160;

led blackLed = led(BLACK_LED_PIN, MAX_BLACK);
led redLed = led(RED_LED_PIN, MAX_RED);
rgb_led xyz = { Y_LED_PIN, X_LED_PIN, Z_LED_PIN , 0, MAX_RGB};


// for code purposes we define a "sensor" to be a function that returns
// some (integer) value, usually corresponding to a reading
// or a function of readings. (Note how neatly this extends to sensors that
// communicate by serial port, for example)

int front_light_sensor () {return analogRead(FRONT_LIGHT_SENSOR_PIN);}
int side_light_sensor () {return analogRead(SIDE_LIGHT_SENSOR_PIN);}
int heat_sensor () {   return analogRead(HEAT_SENSOR_PIN);}
int side_heat_sensor () {return analogRead(SIDE_HEAT_SENSOR_PIN);}
int light_diff_sensor () {return abs(front_light_sensor() - side_light_sensor());}
int light_ratio_sensor () {return front_light_sensor() / side_light_sensor();}
int heat_diff_sensor () {return abs(heat_sensor() - side_heat_sensor());}
int ld() { return front_light.diff();}
int lsd() { return side_light.diff();}

// a stream is an object with a history (so smoothing and time-differences are 
// kept) and a calibration (that determines a sensor's actual range with 
// mean/standard deviation/min and max)

stream front_light = stream(&front_light_sensor);
stream side_light = stream(&side_light_sensor);
stream heat = stream(&heat_sensor);
stream side_heat = stream(&side_heat_sensor);
stream heat_diff = stream(&heat_diff_sensor);
stream light_ratio = stream(&light_ratio_sensor);
stream light_d = stream(&ld);
stream side_light_d = stream(&lsd);


// calibration is performed by the stream's "cal" attribute. 
// but here it's broken in begin/step/end parts so all sensors
// can be simultaneously calibrated within a timespan rather than
// sequentially. This is better because we want calibration to adjust to 
// the state space at a given moment.

void calibrate_streams(unsigned long duration = 5 SECONDS   ){
    unsigned long t0 = millis();
    heat.cal.begin();
    heat_diff.cal.begin();
    front_light.cal.begin();
    side_light.cal.begin();
    side_heat.cal.begin();
    light_ratio.cal.begin();
    light_d.cal.begin();
    side_light_d.cal.begin();

    while (millis() < t0 + duration) {
        heat.cal.step();
        heat_diff.cal.step();
        front_light.cal.step();
        side_light.cal.step();
        side_heat.cal.step();
        light_ratio.cal.step();
        light_d.cal.step();
        side_light_d.cal.step();
    }
    heat.cal.end();
    heat_diff.cal.end();
    front_light.cal.end();
    side_light.cal.end();
    side_heat.cal.end();
    light_ratio.cal.end();
    light_d.cal.end();
    side_light_d.cal.end();

}

void update_streams() {
    front_light.update();
    side_light.update();
    heat.update();
    side_heat.update();
    heat_diff.update();
    light_ratio.update();
    light_d.update();
    side_light_d.update();
}


#endif