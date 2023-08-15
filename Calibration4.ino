#define verbose 1 // I'm using serial communications to "print debug" during development

#include "calstream.h" # provides sensor access
#include "leds.h" # abstraction for LEDs
#include "hw_layout.h" # maps hardware to sensors and leds and provides setup/update utilities

// calibration inervals
const unsigned long initialCalibration = 9 SECONDS; // what? 
const unsigned long runningCalibration = 4 SECONDS;   
const unsigned long recalibrationEvery = 60 SECONDS; 

unsigned long t0 = 0; // restart with t0=millis() after each cycle

float a, b, R, G, B,light_diff;
int u,v, tl;


int total_light(stream* front_light, stream* side_light, stream* light_ratio){
  return (side_light->smoothedValue * light_ratio->cal.center 
              + front_light->smoothedValue) / (light_ratio->cal.center +1);
}


void setup() {
  digitalWrite(BLACK_LED_PIN, HIGH);
  pin_setup();
  xyz.demo();
  calibrate_streams();
  delay(100);
  digitalWrite(BLACK_LED_PIN, LOW);
  t0 = millis();
  
  #ifdef verbose
    Serial.begin(9600);
  #endif
}

#ifdef verbose
  void print_sensor(stream *s) {
      Serial.print(" || sv=");
      Serial.print(s->smoothedValue);
      Serial.print(",mu=");
      Serial.print(s->cal.center);
      Serial.print(",sd=");
      Serial.print(s->cal.spread);
      Serial.print("z=");
      Serial.print(s->zscore());
}
#endif

void loop() {
  if (millis() - t0 > recalibrationEvery) {
    calibrate_streams(3 SECONDS);
    t0 = millis();
  }
  update_streams();
  tl = 80 + front_light.smoothedValue + side_light.smoothedValue;
  light_diff = abs(front_light.smoothedValue - light_ratio.smoothedValue*side_light.smoothedValue);
  
  u = map(front_light.zscore(), 
          1.96,-1.96, 
          0,front_light.cal.max_);
  v = map(side_light.zscore(), 
          2.7,-2.7,
          0, side_light.cal.max_);


  G += 128 * map(heat_diff.bell_curve(10)*side_heat.bell_curve(10) + heat.bell_curve(10),
        1,0,
        0,1);

  G += 255 * map(heat.zscore(), 
          -1.96,1.96,
          0,1) + map(side_heat.zscore(),-1.96,1.96,0,1);

  B = 128 * map(side_light.bell_curve(1)+front_light.bell_curve(1),
          0,1,
          0,1);

  R = 255 * map(light_d.zscore(),
        -1,1,
        0,1);
  R += 255 * map(side_light.smoothedValue,
        0,50,
        0,1);
  
  R += (float) u + (float) v;


  R += front_light.cal.min_;
  G += front_light.cal.min_;
  B += front_light.cal.min_;

  R = ((int) R * tl ) % 255;
  G = ((int) G * tl ) % 255;
  B = ((int) B * tl ) % 255;

  redLed.write(u/4 );
  blackLed.write(v/4 );
  xyz.write_rgb(R,G,B);
  #if verbose
    Serial.print("FL");
    print_sensor(&front_light);
    Serial.print("::LD");
    print_sensor(&light_d);
    Serial.print("::H");
    print_sensor(&side_heat);
    Serial.print(":: RGB ");
    Serial.print( R);
    Serial.print(" : ");
    Serial.print( G);
    Serial.print(" : ");
    Serial.print( B);


    Serial.println("");
  #endif
  delay(50);
}
