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
const float BEND_RESISTANCE = 37000.0; // resistance at 180 deg

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


//vars for flashing
long startFlash;
long lastFlash;
bool isOn = true;
bool isFlashing = false;
bool isFinished = false;

const int timeToHold = 3000;
const int flashBuffer = 15;

int aimAngle = 90;
const int greenBuffer = 50;


void loop() 
{
  // Read the ADC, and calculate voltage and resistance from it
  int flexADC = analogRead(FLEX_PIN);
  float flexV = flexADC * VCC / 1023.0;
  float flexR = R_DIV * (VCC / flexV - 1.0);
  float curAngle = map(flexR, STRAIGHT_RESISTANCE, BEND_RESISTANCE, 0, 180.0);
  
  //rolling average
  angle = angle + (curAngle-angle)/filterWeight;

  Serial.println(angle);

  float diff = abs(aimAngle-angle);
  
  if(diff < flashBuffer){
    //start
    if(!isFlashing){
      isFlashing = true;
      startFlash = millis();
      lastFlash = millis();
    }

    int sinceStart = (millis()-startFlash);
    int sinceLast = (millis()-lastFlash);
    
    //if been holding for long enough, finish
    if(timeToHold < sinceStart){
      //isFinished = true;
      aimAngle = random(10, 170);
    //period of swapping increaces over time
    }else if(map(sinceStart, timeToHold, 0, 0, 600) < sinceLast){
      isOn = !isOn;
      lastFlash = millis();
    }
  }else if(isFlashing){
    isFlashing = false;
    isOn = true;
  }

  fract8 greenFract = (fract8)constMap(diff, greenBuffer, 0);

  for(int i = 0; i < NUM_LEDS; i++){
    if(isFinished){
      leds[i] = CRGB::White;
    }else if(isOn){
      int low = i*180/NUM_LEDS;
      int high = (i+1)*180/NUM_LEDS;
    
      //leds[i] = CRGB(constMap(max(aimAngle, angle), low, high), constMap(min(aimAngle, angle), low, high), 0);
      leds[i] = CRGB(constMap(aimAngle, low, high), 0, 0);
      if(greenFract)
        leds[i] = blend(leds[i], CRGB::Green, greenFract);
    } else
      leds[i] = CRGB::Black;
  }

  FastLED.show();
}


int constMap(float val, float low, float high){
  return constrain(map(val, low, high, 0, 255), 0, 255);
}

