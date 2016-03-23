/*
  SoundSampler

 created 19 March. 2016
 by Toon Nelissen (cinezaster)

 This code is in the public domain.

 */
#include <Adafruit_NeoPixel.h>
#include <Encoder.h>
 
#define LED_STRIP 16
#define LED_PIN 6

#define ENCODER_PIN_1 7
#define ENCODER_PIN_2 8

#define MICROPHONE_PIN A0
#define STEPS_METER 340


Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_STRIP, LED_PIN, NEO_GRB + NEO_KHZ800);
Encoder myEnc(ENCODER_PIN_1, ENCODER_PIN_2);

uint16_t sensorValue = 0;
int soundSampleArray[200];
uint16_t amountOfSoundSamples;

uint16_t calDif;
uint16_t calMid;

long oldPosition  = 0;

void setup() {
  Serial.begin(9600);
  /*while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }*/
  Serial.println("////Wave length printer //////");
  
  analogReadResolution(12);
  
  strip.begin();
  strip.show();
}

void loop() {
  Serial.println("Ready for Recording");
  
  setNeoPixelStick(10,5,1);
  
  resetSoundwaveSampleArray();
  
  myEnc.write(0);
  oldPosition = 0;
  while(oldPosition < 20 && oldPosition > -20) {
    long newPosition = myEnc.read();
    if (newPosition != oldPosition) {
      oldPosition = newPosition;
    }
  }
  
  readSoundWaveSample();
  
  setNeoPixelStick(0,40,0);

  delay(3000);

  setNeoPixelStick(0,0,0);
  
  myEnc.write(0);
  oldPosition = 0;
  while(oldPosition < STEPS_METER) {
    long newPosition = myEnc.read();
    if (newPosition != oldPosition) {
      oldPosition = newPosition;
      
      int myArrayPos = map(newPosition, 0, STEPS_METER, 0, amountOfSoundSamples);
      int rainbowPos = map(newPosition, 0, STEPS_METER, 0, 255);
      
      int outputValue = map(soundSampleArray[myArrayPos], calMid-calDif, calMid+calDif, 0, strip.numPixels());
      
      for(uint16_t i = 0; i < strip.numPixels(); i++) {
        if (i == outputValue) {
          strip.setPixelColor(i,Wheel(rainbowPos & 255));
        } else if (i < 8 + abs(8-outputValue) && i > 8-abs(8-outputValue)){
          strip.setPixelColor(i,40,30,40);
        } else {
          strip.setPixelColor(i,0,0,0);
        }
      }
      strip.show();
      
    }
  }
  setNeoPixelStick(0,0,0);
  Serial.println("done writing");
  delay(4000);
}

void setNeoPixelStick (uint8_t red, uint8_t green, uint8_t blue) {
  for(uint16_t i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i,red,green,blue);
  }
  strip.show();
}

void resetSoundwaveSampleArray() {
  for (int z = 0; z < 200; z++){
    soundSampleArray[z] = 0;
  }
}

void readSoundWaveSample () {
  Serial.println("start reading");
  int maxR = 0;
  int minR = 4096;
  long totalCal = 0;
  int calRead;
  
  unsigned int startCounting = micros() + 2915;
  int counter = 0;
  while(micros() < startCounting ){
    calRead = analogRead(MICROPHONE_PIN);
    soundSampleArray[counter] = calRead;

    if (calRead > maxR) {
      maxR = calRead;
    }
    if (calRead < minR) {
      minR = calRead;
    }
    totalCal = totalCal + calRead;
    
    counter++;
    delayMicroseconds(15);
  }

  calDif = ((maxR - minR)/2);
  calMid = totalCal/counter;
  
  Serial.println("done reading");
  amountOfSoundSamples = counter;
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}
