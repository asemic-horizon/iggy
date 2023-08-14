#include "leds.h"

led::led(uint8_t p, int maxb) {
    pin = p;
    max_brightness = maxb;
}

void led::update() {
    analogWrite(pin, value);
}

void led::write(int v) {
    value = v;
    update();
}

int led::write_map(int v, int vmin, int vmax) {
    value = map(v, vmin, vmax, min_brightness, max_brightness);
    update();
}

void led::set_brightness(stream* s) {
    int range = s->cal.max - s->cal.min;
    max_brightness = 255 * (s->smoothedValue - s->cal.min) / range;
    min_brightness = 255 * (s->cal.max - s->smoothedValue) / range;
}

int led::z_write(stream* s) {
    write_map(s->smoothedValue, s->cal.min, s->cal.max);
}

int led::bell_write(stream* s) {
    int value = map(s->bell_curve(), 0, 1, min_brightness, max_brightness);
    write(value);
}

void rgb_led::write_rgb(int red, int green, int blue) {
    analogWrite(red_pin, red);
    analogWrite(green_pin, green);
    analogWrite(blue_pin, blue);
}

void rgb_led::write_ita(int intensity, int terra, int aqua) {
    int blue = 3 * intensity - terra;
    int red = 3 * intensity - aqua;
    int green = terra + aqua - 3 * intensity;
    write_rgb(red, green, blue);
}

void rgb_led::demo() {
    for (int cy = 0; cy <= 255; cy += 50) {
        for (int cz = 0; cz <= 255; cz += 50) {
            write_rgb(0, cy, cz);
        }
    }

    for (int cx = 0; cx <= 255; cx += 50) {
        for (int cy = 0; cy <= 255; cy += 50) {
            write_rgb(cx, cy, 0);
        }
    }

    for (int cx = 0; cx <= 255; cx += 50) {
        for (int cz = 0; cz <= 255; cz += 50) {
            write_rgb(cx, 0, cz);
        }
    }
    delay(2);
    write_rgb(0, 0, 0);
}
