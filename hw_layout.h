#ifndef HW_LAYOUT
#define HW_LAYOUT
#include "calstream.h"
#include "leds.h"

#define SECONDS * 1000


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
  pinMode(FRONT_LIGHT_SENSOR_PIN, INPUT);
  pinMode(SIDE_LIGHT_SENSOR_PIN, INPUT);
  pinMode(HEAT_SENSOR_PIN, INPUT);
  pinMode(SIDE_HEAT_SENSOR_PIN, INPUT);

  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BLACK_LED_PIN, OUTPUT);
  pinMode(SERVO_PIN, OUTPUT);
}

int front_light_sensor () {return analogRead(FRONT_LIGHT_SENSOR_PIN);}
int side_light_sensor () {return analogRead(SIDE_LIGHT_SENSOR_PIN);}
int heat_sensor () {   return analogRead(HEAT_SENSOR_PIN);}
int side_heat_sensor () {return analogRead(SIDE_HEAT_SENSOR_PIN);}
int light_diff_sensor () {return abs(front_light_sensor() - side_light_sensor());}
int light_ratio_sensor () {return front_light_sensor() / side_light_sensor();}
int heat_diff_sensor () {return abs(heat_sensor() - side_heat_sensor());}

stream front_light = stream(&front_light_sensor);
stream side_light = stream(&side_light_sensor);
stream heat = stream(&heat_sensor);
stream side_heat = stream(&side_heat_sensor);
stream heat_diff = stream(&heat_diff_sensor);
stream light_ratio = stream(&light_ratio_sensor);

void calibrate_streams(unsigned long duration = 5 SECONDS   ){
    heat.calibrate(duration);
    heat_diff.calibrate(duration);
    front_light.calibrate(duration);
    side_light.calibrate(duration);
    side_heat.calibrate(duration);
    light_ratio.calibrate(duration);
}

void update_streams() {
    front_light.update();
    side_light.update();
    heat.update();
    side_heat.update();
    heat_diff.update();
    light_ratio.update();
}
const int MAX_BLACK = 255;
const int MAX_RED = 64;
const int MAX_RGB = 160;

led blackLed = led(BLACK_LED_PIN, MAX_BLACK);
led redLed = led(RED_LED_PIN, MAX_RED);
rgb_led xyz = { Y_LED_PIN, X_LED_PIN, Z_LED_PIN , 0, MAX_RGB};


#endif