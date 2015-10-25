/*
2015-10-25:
Testing audio signal max value. Using ADC interval 5V reference,
connected audio amplifier, biasing signal at 2.5V (ADC val - 512).
Max audio ADC signal abs from bias point is 366 [abs(512-adcVal)].
It is 51dB, having 15 LEDs, it is 3.4 dB per LED
*/


#include <math.h>

void setup() {
  pinMode(13, OUTPUT);
}

void loop() {
  int adcVal = analogRead(A0);
  int audioVal = abs(512-adcVal);

  if(audioVal > 366){
    digitalWrite(13, HIGH);
  }
  else{
    digitalWrite(13, LOW);
  }
}
