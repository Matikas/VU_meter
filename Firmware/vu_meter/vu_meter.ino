/*
 https://learn.adafruit.com/led-ampli-tie/the-code
 */
 
#include <Adafruit_NeoPixel.h>
#include <math.h>
 
#define N_PIXELS  29  // Number of pixels in strand
#define N_STEPS   14
#define MIC_PIN   A0  // Microphone is attached to this analog pin
#define LED_PIN    6  // NeoPixel LED strand is connected to this pin
#define SAMPLE_WINDOW   30  // Sample window for average level
#define INPUT_FLOOR 60 //Lower range of analogRead input
#define INPUT_CEILING 480 //Max range of analogRead input, the lower the value the more sensitive (1023 = max)
 
 
byte peak = 14;      // Peak level of column; used for falling dots
unsigned int sample;
 
byte dotCount = 0;  //Frame counter for peak dot
byte dotHangCount = 0; //Frame counter for holding peak dot
 
Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);



enum mode{
 classic = 0,
 rainbow = 1,
 potentiometer = 2
};

mode volatile currentMode;

void setup() 
{
  // This is only needed on 5V Arduinos (Uno, Leonardo, etc.).
  // Connect 3.3V to mic AND TO AREF ON ARDUINO and enable this
  // line.  Audio samples are 'cleaner' at 3.3V.
  // COMMENT OUT THIS LINE FOR 3.3V ARDUINOS (FLORA, ETC.):
  //  analogReference(EXTERNAL);
 
  // Serial.begin(9600);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'






  //TODO: set mode (read from EEPROM, if not found - set default)
  currentMode = potentiometer;
}
 
void loop() 
{
  unsigned long startMillis= millis();  // Start of sample window
  float peakToPeak = 0;   // peak-to-peak level
 
  unsigned int signalMax = 0;
  unsigned int signalMin = 1023;
  unsigned int c, y;
 
  // collect data for length of sample window (in mS)
  while (millis() - startMillis < SAMPLE_WINDOW)
  {
    sample = analogRead(MIC_PIN);
    if (sample < 1024)  // toss out spurious readings
    {
      if (sample > signalMax)
      {
        signalMax = sample;  // save just the max levels
      }
      else if (sample < signalMin)
      {
        signalMin = sample;  // save just the min levels
      }
    }
  }
  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
 
 


  switch(currentMode){
    case classic: 
      drawClassic();
      break;
     case rainbow: 
      drawRainbow();
      break;
     case potentiometer:
      drawPotentiometer();
      break;
     default: { }
  }
 

 
  //Scale the input logarithmically instead of linearly
  c = fscale(INPUT_FLOOR, INPUT_CEILING, N_STEPS, 0, peakToPeak, 1);
 
  
 
 

  if (c <= N_STEPS) { // Fill partial column with off pixels
    for(int i=N_STEPS-c; i<=N_STEPS; i++){   
      strip.setPixelColor(i, 0, 0, 0);
      strip.setPixelColor(N_PIXELS-1-i, 0, 0, 0);
    }
  }

  strip.show();
}





void drawClassic(){
    for (int i=0;i<=N_STEPS;i++){
      if(i<N_STEPS*0.5){
         strip.setPixelColor(i, 0, 255, 0);
         strip.setPixelColor(N_PIXELS-i, 0, 255, 0);
      }
      else if(i>=N_STEPS*0.5 && i<N_STEPS*0.7){
         strip.setPixelColor(i, 255, 255, 0);
         strip.setPixelColor(N_PIXELS-i, 255, 255, 0);
      }
      else{
         strip.setPixelColor(i, 255, 0, 0);
         strip.setPixelColor(N_PIXELS-i, 255, 0, 0);
      }
  }
}

void drawPotentiometer(){
  //TODO: read value from potentiometer
  unsigned int potValue = 400;

  byte redVal = (potValue >> 2) & 0xE0;
  byte grnVal = (potValue << 1) & 0x78;
  byte bluVal = (potValue << 5) & 0xE0;
  
  for (int i=0;i<=N_STEPS;i++){
    strip.setPixelColor(i, redVal, grnVal, bluVal);
    strip.setPixelColor(N_PIXELS-i, redVal, grnVal, bluVal);
  }
}

void drawRainbow(){
  for (int i=0;i<=N_STEPS;i++){
    strip.setPixelColor(i,Wheel(map(i, 0, N_STEPS, 30, 150)));
    strip.setPixelColor(N_PIXELS-i,Wheel(map(i, 0, N_STEPS, 30, 150)));
  }
}




 
float fscale( float originalMin, float originalMax, float newBegin, float
newEnd, float inputValue, float curve){
 
  float OriginalRange = 0;
  float NewRange = 0;
  float zeroRefCurVal = 0;
  float normalizedCurVal = 0;
  float rangedValue = 0;
  boolean invFlag = 0;
 
 
  // condition curve parameter
  // limit range
 
  if (curve > 10) curve = 10;
  if (curve < -10) curve = -10;
 
  curve = (curve * -.1) ; // - invert and scale - this seems more intuitive - postive numbers give more weight to high end on output 
  curve = pow(10, curve); // convert linear scale into lograthimic exponent for other pow function
 
  /*
   Serial.println(curve * 100, DEC);   // multply by 100 to preserve resolution  
   Serial.println(); 
   */
 
  // Check for out of range inputValues
  if (inputValue < originalMin) {
    inputValue = originalMin;
  }
  if (inputValue > originalMax) {
    inputValue = originalMax;
  }
 
  // Zero Refference the values
  OriginalRange = originalMax - originalMin;
 
  if (newEnd > newBegin){ 
    NewRange = newEnd - newBegin;
  }
  else
  {
    NewRange = newBegin - newEnd; 
    invFlag = 1;
  }
 
  zeroRefCurVal = inputValue - originalMin;
  normalizedCurVal  =  zeroRefCurVal / OriginalRange;   // normalize to 0 - 1 float
 
  // Check for originalMin > originalMax  - the math for all other cases i.e. negative numbers seems to work out fine 
  if (originalMin > originalMax ) {
    return 0;
  }
 
  if (invFlag == 0){
    rangedValue =  (pow(normalizedCurVal, curve) * NewRange) + newBegin;
 
  }
  else     // invert the ranges
  {   
    rangedValue =  newBegin - (pow(normalizedCurVal, curve) * NewRange); 
  }
 
  return rangedValue;
}

 
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } 
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
