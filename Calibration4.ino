#define verbose 1 // I'm using serial communications to "print debug" during development

#include "calstream.h" 
#include "leds.h" 
#include "hw_layout.h" 

// calibration inervals
const unsigned long initialCalibration = 30 SECONDS; // what? 
const unsigned long runningCalibration = 8 SECONDS;   
const unsigned long recalibrationEvery = 120 SECONDS; 

unsigned long t0 = 0; // restart with t0=millis() after each cycle

float U,V,W, R, G, B,light_diff, tl;


int total_light(stream* front_light, stream* side_light, stream* light_ratio){
  return (side_light->smoothedValue * light_ratio->cal.center 
              + front_light->smoothedValue) / (light_ratio->cal.center +1);
}


void setup() {
  Serial.begin(9600);
  
  digitalWrite(BLACK_LED_PIN, HIGH);
  pin_setup();
  xyz.demo();
  calibrate_streams(initialCalibration);
  delay(100);
  digitalWrite(BLACK_LED_PIN, LOW);
  t0 = millis();
  
  
}

#ifdef verbose
  void print_sensor(stream *s) {
      Serial.print("=");
      Serial.print(s->relative());
}
#endif

void loop() {
  if (millis() - t0 > recalibrationEvery) {
    calibrate_streams(runningCalibration);
    t0 = millis();
  }
  update_streams();
  tl = 80 + front_light.smoothedValue + side_light.smoothedValue;
  light_diff = abs(front_light.smoothedValue - light_ratio.smoothedValue*side_light.smoothedValue);
  


  U = 256 * map(heat_d.bell_curve(10)*side_heat.bell_curve(10) + heat.bell_curve(10),
        1,0,
        0,1);


  V = 128 * map(heat_d.relative(),
          1,0,
          0,1);

  W = 0;
  

  B = map(light_d.relative(),
          1,0,
          0,tl);

  B += map(side_light.relative(),
          1, 0,
          0,tl);
  
  B/=2;
  R = map(front_light.relative(),
          1, 0,
          0,tl);
          
  G = map(side_heat.relative(),
          0, 1,
          0,tl);
  
  G += map(heat.relative(),
          0,1,
          0,tl);
  G += map(front_light.spread(),
          0,10,
          0,tl);
  B -=map(front_light.spread(),
          0,15,
          0,tl);
    R -=map(front_light.spread(),
          0,10,
          0,tl);
  G += R/4;
  R -= R/4;
  R = ((int) R  );
  G = ((int) G  );
  B = ((int) B );

  xyz.write_rgb(R,G, B);
  abc.write_rgb(0,U,V);
  #if verbose
    Serial.print("FH");
    print_sensor(&heat);
    Serial.print(" SH");
    print_sensor(&side_heat);
    Serial.print(" FL");
    print_sensor(&front_light);
    Serial.print(" SL");
    print_sensor(&side_light);
    Serial.print("::LD");
    print_sensor(&light_d);
    Serial.print("::H");
    Serial.print(DHT.humidity);
    Serial.print(DHT.temperature);
    Serial.print(":: RGB ");
    Serial.print( R);
    Serial.print(" : ");
    Serial.print( G);
    Serial.print(" : ");
    Serial.print( B);


    Serial.println("");
  #endif
  delay(15);
}
