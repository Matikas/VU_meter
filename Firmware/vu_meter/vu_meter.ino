/*
2015-10-25:
Testing audio signal max value. Using ADC interval 5V reference,
connected audio amplifier, biasing signal at 2.5V (ADC val - 512).
Max audio ADC signal abs from bias point is 366 [abs(512-adcVal)].
It is 51dB, having 15 LEDs, it is 3.4 dB per LED
*/


#include <math.h>
#include <Adafruit_NeoPixel.h>

#define PIN_LEFT 6
#define ADC_LEFT A0
#define TOTAL_PIXELS 29


Adafruit_NeoPixel stripLeft = Adafruit_NeoPixel(TOTAL_PIXELS, PIN_LEFT, NEO_GRB + NEO_KHZ800);
float dbLeft;
float const Zero = 512;
float const dbPerLed = 2.1;
int const Steps = TOTAL_PIXELS % 2 > 0 ? (TOTAL_PIXELS / 2) + 1 : (TOTAL_PIXELS / 2);


void setup() {
  stripLeft.begin();
  stripLeft.show(); // Initialize all pixels to 'off'
}

void loop() {
  getPPMsample();

  drawClassic();
}


void drawClassic(){
  for(int led = 0; led < Steps; led++){
    if(abs(dbLeft) >= (led*dbPerLed)){
      stripLeft.setPixelColor(led, 0, 255, 0);
      stripLeft.setPixelColor(TOTAL_PIXELS - led, 0, 255, 0);
    }
    else{
      stripLeft.setPixelColor(led, 0, 0, 0);
      stripLeft.setPixelColor(TOTAL_PIXELS - led, 0, 0, 0);
    }
  }
  stripLeft.show();
}


void getPPMsample(){
  float maxAudio = 0; float rawAudio;

  for(int sample = 0; sample < 48; sample++){
    rawAudio = analogRead(ADC_LEFT);
    if(rawAudio > maxAudio){ maxAudio = rawAudio; }
  }
  
  dbLeft = 20 * log10(abs(maxAudio-Zero)/Zero);
}

