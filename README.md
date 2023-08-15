# Arduino project template

This is a template for Arduino projects that integrate multiple sensors and LEDs. (Servo control functionality coming soon).

## Usage

You'll need at a minimum to modify `hw_layout.h`:

* Declare named constants for your pin numbers (to protect your mental health)
* Declare `led` objects from your LEDs. This is as simple as `led redLed = led(RED_LED_PIN, MAX_RED);` or `rgb_led xyz = { X_LED_PIN, Y_LED_PIN, Z_LED_PIN , 0, MAX_RGB};`.
* Declare your sensor functions; sometimes just `int heat_sensor () {   return analogRead(HEAT_SENSOR_PIN);}`, but also allowing for combinations of sensor values and digital sensors read through serial communication.
* Initialize value streams: `stream heat = stream(&heat_sensor);`. Streams have a short history enabling time differences and smoothing, as well as a calibration object.
* Calibrate: `t0 = millis(); heat.cal.begin(); while millis()<t0+time_you_want {heat.cal.step();} heat.cal.end()`. (In this template we calibrate several sensors together in a `calibrate_streams()` utility.)


Your sketch file (ending in `.ino`) can then be quite simple:

```cpp
#define SECONDS * 1000

#include "calstream.h" # provides sensor access
#include "leds.h" # abstraction for LEDs
#include "hw_layout.h" # maps hardware to sensors and leds and provides setup/update utilities

unsigned long t0 = 0; // restart with t0=millis() after each cycle
float R, G, B;

void setup() {
  redLed.write(255); // just for visual feedback
  pin_setup();
  calibrate_streams(10 SECONDS);
  redLed.write(0);
  t0 = millis();
  
void loop() {
  if (millis() - t0 > 120 SECONDS) {
    calibrate_streams(3 SECONDS);
    t0 = millis();
  }
  heat.update(); // but note the update_sensors() utility in hw_layout.h
  R += 255 * map(heat.zscore(), 
          -1.96,1.96,
          0,1) + map(side_heat.zscore(),-1.96,1.96,0,1);

  G = 255 * map(heat.bell_curve(),
          0, 1,
          0,1);

  B = 255 * map(heat.diff(),
          -20, 20,
          0, 1);
  xyz.write_rgb(R,G,B);
  delay(50);
}
```

## Structure

Besides the sketch, there are three groups of files in this template:

* `hw_layout.h` -- is a file you must customize to map  your hardware to  useful `stream` and `led`,`rgb_led` objects.
* `leds.h` and `leds.cpp` -- paricularly useful for RGB leds, but can serve as starter code for your own ideas
* `calstream.h` and `calstream.cpp`

## License

This is a "learning in public" type of project, licensed by the SILLY PUBLIC LICENSE: silly usage is governed by an Apache 2.0 license, while serious usage is not authorized. It should, however, help the casual Arduino user to take the leap into cleaner code and functionality encapsulation. 
