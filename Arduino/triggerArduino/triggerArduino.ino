#include <Wire.h>
#include "SparkFun_VEML6030_Ambient_Light_Sensor.h"

#define AL_ADDR 0x48

SparkFun_Ambient_Light light(AL_ADDR);


// LOW = 24V, HIGH = 0V
#define analogTrigger 4
#define analogLine 12

// LOW = 24V = OFF, HIGH = 0V = ON
#define npnTrigger 2
#define npnLine 8

// LOW = 24V = ON, HIGH = 0V = OFF
#define pnpTrigger 3
#define pnpLine 6


#define delayVal 100
#define lightTime 500

boolean analogTriggerState = false;
boolean pnpTriggerState = false;
boolean npnTriggerState = false;

boolean pnpPrev = false;
boolean npnPrev = false;




void setup() {
  Serial.begin(9600);
  pinMode(analogLine, OUTPUT);
  pinMode(npnLine, OUTPUT);
  pinMode(pnpLine, OUTPUT);

  pinMode(analogTrigger, INPUT);
  pinMode(npnTrigger, INPUT_PULLUP);
  pinMode(pnpTrigger, INPUT_PULLUP);
  
  digitalWrite(analogLine, LOW);
  digitalWrite(npnLine, LOW);
  digitalWrite(pnpLine, HIGH);
}

void loop() {
  analogTriggerState = !digitalRead(analogTrigger);
  pnpTriggerState = digitalRead(pnpTrigger);
  npnTriggerState = digitalRead(npnTrigger);

  Serial.println(pnpTriggerState);

  digitalWrite(analogLine, analogTriggerState);
  if (!pnpTriggerState) {
    delay(delayVal);
    digitalWrite(pnpLine, LOW);
    delay(lightTime);
    digitalWrite(pnpLine, HIGH);
    pnpPrev = true;
  }else if (pnpTriggerState) {
    pnpPrev = false;
  }


  if (!npnTriggerState) {
    delay(delayVal);
    digitalWrite(npnLine, HIGH);
    delay(lightTime);
    digitalWrite(npnLine, LOW);
    npnPrev = true;
  }else if (npnTriggerState) {
    npnPrev = false;
  }
}
