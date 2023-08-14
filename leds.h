#ifndef LEDS_H
#define LEDS_H

#include <Arduino.h>
#include "calstream.h"

struct led {
    uint8_t pin;
    int min_brightness;
    int max_brightness;
    int value;

    led(uint8_t p, int maxb = 255);

    void update();
    void write(int v);
    int write_map(int v, int vmin, int vmax);
    void set_brightness(stream* s);
    int z_write(stream* s);
    int bell_write(stream* s);
};

struct rgb_led {
    uint8_t red_pin;
    uint8_t green_pin;
    uint8_t blue_pin;
    int min_brightness;
    int max_brightness;

    void write_rgb(int red, int green, int blue);
    void write_ita(int intensity, int terra, int aqua);
    void demo();
};

#endif
