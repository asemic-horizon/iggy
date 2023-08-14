#define verbose 1
#define SECONDS * 1000
#include "calstream.h"
#include "leds.h"
#include "hw_layout.h"

// calibration inervals
const unsigned long initialCalibration = 9 SECONDS   ; 
const unsigned long runningCalibration = 5 SECONDS  ;   
const unsigned long recalibrationEvery = 300 SECONDS; 

unsigned long t0 = 0; // restart with t0=millis() after each cycle

float a, b, R, G, B,light_diff, tl;
int u,v;


int total_light(stream* front_light, stream* side_light, stream* light_ratio){
  return (side_light->smoothedValue * light_ratio->cal.center 
              + front_light->smoothedValue) / (light_ratio->cal.center +1);
}


void setup() {
  digitalWrite(SERVO_PIN, HIGH);
  pin_setup();
  xyz.demo();
  delay(100);
  digitalWrite(SERVO_PIN, LOW);
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
      Serial.print("phi=");
      Serial.print(s->bell_curve());
}
#endif

void loop() {
  if (millis() - t0 > recalibrationEvery) {
    calibrate_streams();
    t0 = millis();
  }
  update_streams();
  tl = total_light(&front_light, &side_light, &light_ratio);
  tl = 4*tl/7;
  light_diff = abs(front_light.smoothedValue - light_ratio.smoothedValue*side_light.smoothedValue);
  
  u = map(front_light.zscore(), 
          1.96,-1.96, 
          0,front_light.cal.max_);
  v = map(side_light.zscore(), 
          1.96,-1.96,
          0, side_light.cal.max_);

  R = map(light_ratio.bell_curve(),
        1.96*light_ratio.cal.spread,0,
        0,tl/2);

  R += map(abs(heat_diff.bell_curve()),
        1.96*heat.cal.spread,0,
        0,tl/2);

  B = map(abs(side_light.bell_curve(3)+front_light.bell_curve()),
          0,5,
          0,tl/3);
  B += map(side_heat.zscore(),
        1.96,-1.96,
        0,tl/3);


  G = map(abs(front_light.diff()),
        0,2*front_light.cal.spread,
        tl/3,tl/2);
  G += map(abs(side_light.diff()),
        tl/3,side_light.cal.spread,
        0,tl/3);

  redLed.write(u % 255);
  blackLed.write(v % 255);
  xyz.write_rgb(max(0,R+15),max(0,G-40),max(0,B-80));
  #if verbose
    Serial.print("FL");
    print_sensor(&front_light);
    Serial.print("::SL");
    print_sensor(&side_light);
    Serial.print("::H");
    print_sensor(&side_heat);
    Serial.print(":: RGB ");
    Serial.print((int) R);
    Serial.print(" : ");
    Serial.print((int) G);
    Serial.print(" : ");
    Serial.print((int) B);


    Serial.println("");
  #endif
  delay(50);
}
