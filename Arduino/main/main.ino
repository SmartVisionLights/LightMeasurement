#include <Wire.h>
#include "SparkFun_VEML6030_Ambient_Light_Sensor.h"

#define AL_ADDR 0x48

SparkFun_Ambient_Light light(AL_ADDR);

// Possible values: .125(1/8), .25(1/4), 1, 2
// Both .125 and .25 should be used in most cases except darker rooms.
// A gain of 2 should only be used if the sensor will be covered by a dark
// glass.
float gain = .125;

// Possible integration times in milliseconds: 800, 400, 200, 100, 50, 25
// Higher times give higher resolutions and should be used in darker light. 
int time = 100;
long luxVal = 0; 

int delayVal = 800;

int calibrationCycles = 20;

String measure = "";
String numOfReadings = "";
String cellGain = "";
String cellTime = "";
String calibrate = "";

double calibrationFactor;


void setup() {
  Wire.begin();
  Serial.begin(256000); // the bigger number the better
  light.begin();
  cellGain = readCell("K2");
  cellTime = readCell("L2");
  gain = cellGain.toInt();
  gain /= 1000.0;

  time = cellTime.toInt();
  
  // Again the gain and integration times determine the resolution of the lux
  // value, and give different ranges of possible light readings. Check out
  // hoookup guide for more info. 
  light.setGain(gain);
  light.setIntegTime(time);
  float gainVal = light.readGain();
  int timeVal = light.readIntegTime();
  Serial.println("RESETTIMER"); //resets timer to 0
}

void loop() {
  // Monitor Activation Cell
  measure = readCell("E2");
  calibrate = readCell("G2");
  
  if (measure.equals("Measuring")){
    numOfReadings = readCell("F2");
    
    for (int i = 0; i < 100; i++) {
      String location = "A";
      location += (2 + i);
      if (i > numOfReadings.toInt()) {
        writeCell(location,"");
      }else{
        luxVal = light.readLight();
        writeCell(location, String(luxVal-calibrationFactor));
        delay(delayVal);
      }
    }
    writeCell("E2", "Measure");
  }

  if (calibrate.equals("Calibrating")) {
    calibrationFactor = calibrateSensor();
    writeCell("H2", String(calibrationFactor));
    writeCell("G2", String("Calibrate"));
  }
  
}

double calibrateSensor() {
  cellGain = readCell("K2");
  cellTime = readCell("L2");
  
  gain = cellGain.toInt();
  gain /= 1000.0;
  
  time = cellTime.toInt();

  light.setGain(gain);
  light.setIntegTime(time);
  
  int measurements[calibrationCycles];

  double total = 0;
  
  for (int i = 0; i < calibrationCycles; i++) {
    luxVal = light.readLight();
    measurements[i] = luxVal;
    total += luxVal;
  }

  double average = total/calibrationCycles;
  return average;
}


void writeCell(String cell, String value) {
  Serial.print("CELL,SET," + cell + ",");
  Serial.println(value);
}

String readCell(String cell) {
  Serial.println("CELL,GET," + cell + ",");
  String cellData = "";
  delay(50);
  if (Serial.available() > 0) {
    cellData = Serial.readString();
    cellData.trim();
  }
  return cellData;
}
