#include <FastLED.h>

#define NUM_LEDS 3

const int FLEX_PIN = A0; // Pin connected to voltage divider output

// Measure the voltage at 5V and the actual resistance of your
// 47k resistor, and enter them below:
const float VCC = 5; // Measured voltage of Ardunio 5V line
const float R_DIV = 47500.0; // Measured resistance of 3.3k resistor

// Upload the code, then try to adjust these values to more
// accurately calculate bend degree.
const float STRAIGHT_RESISTANCE = 12200.0; // resistance when straight
const float BEND_RESISTANCE = 33000.0; // resistance at 180 deg

const int filterWeight = 32;

float angle;

CRGB leds[NUM_LEDS];

void setup() 
{
  Serial.begin(9600);
  pinMode(FLEX_PIN, INPUT);
  FastLED.addLeds<WS2811, 2, BRG>(leds, NUM_LEDS);
  //FastLED.setCorrection(TypicalLEDStrip);
  //FastLED.setTemperature(OvercastSky);
}

void loop() 
{
  // Read the ADC, and calculate voltage and resistance from it
  int flexADC = analogRead(FLEX_PIN);
  float flexV = flexADC * VCC / 1023.0;
  float flexR = R_DIV * (VCC / flexV - 1.0);

  float curAngle = map(flexR, STRAIGHT_RESISTANCE, BEND_RESISTANCE, 0, 180.0);
  //rolling average
  angle = angle + (curAngle-angle)/filterWeight;

  for(int i = 0; i < NUM_LEDS; i++){
    int low = i*180/NUM_LEDS;
    int high = (i+1)*180/NUM_LEDS;

    if(low <= angle && angle <= high && false){
      leds[i].r = constMap(90, low, high);
      leds[i].g = constMap(angle, low, high);
      leds[i].b = min(leds[i].r, leds[i].g);
    } else {
      leds[i] = CRGB::White;
    }

//    leds[i].setHSV(255*millis()/2000,255,255);
    
//    int low = i*180/NUM_LEDS;
//    int high = (i+1)*180/NUM_LEDS;
//    int val = max(map(average, low, high, 0, 255),0);
//    
//    leds[i].setHSV(255*average/180, 255, min(val, 255));
  }
  
  Serial.print(angle);
  Serial.print('\t');
  Serial.println(90);

  FastLED.show();
}


int constMap(float val, float low, float high){
  return constrain(map(val, low, high, 0, 255), 0, 255);
}

