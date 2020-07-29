#include <Wire.h>
#include "SparkFun_VEML6030_Ambient_Light_Sensor.h"

#define AL_ADDR 0x48

SparkFun_Ambient_Light light(AL_ADDR);


// LOW = 24V, HIGH = 0V
#define analogLine 12



// LOW = 24V = OFF, HIGH = 0V = ON
#define npnLine 8

// LOW = 24V = ON, HIGH = 0V = OFF
#define pnpLine 6




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



String actionCell = "C3";
String barcodeCell = "B3";
String barcodeCell_2 = "B13";


String action = "";
String measure = "";
String numOfReadings = "";
String cellGain = "";
String cellTime = "";
String calibrate = "";

double calibrationFactor = 0.0;

boolean averageWritten = false;

void setup() {
  Wire.begin();
  Serial.begin(256000); // the bigger number the better
  light.begin();
  cellGain = readCell("Q2");
  cellTime = readCell("R2");
  gain = cellGain.toInt();
  gain /= 1000.0;

  time = cellTime.toInt();
  
  // Again the gain and integration times determine the resolution of the lux
  // value, and give different ranges of possible light readings. Check out
  // hookup guide for more info.
  light.setGain(gain);
  light.setIntegTime(time);
  float gainVal = light.readGain();
  int timeVal = light.readIntegTime();
  Serial.println("RESETTIMER"); //resets timer to 0

  pinMode(analogLine, OUTPUT);
  pinMode(npnLine, OUTPUT);
  pinMode(pnpLine, OUTPUT);

  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  
  digitalWrite(analogLine, HIGH);
  digitalWrite(npnLine, HIGH);
  digitalWrite(pnpLine, HIGH);

}

void loop() {
  action = readCell(actionCell);
  delay(100);
  if (action.equals("S")){
    writeCell(barcodeCell, "SL");
  }else if (action.equals("SL")) {
    writeCell(barcodeCell, "M"); 
  }else if (action.equals("M")){
    measureLight(false);
    measureLight(true);
  }else if (action.equals("A")) {
    writeCell(actionCell, "M");
  }else if (action.equals("")) {
    writeCell(barcodeCell, "S");
  }
}


int start = 0;

void measureLight(boolean overdrive) {
  if (overdrive) {
    digitalWrite(analogLine, LOW);
    averageWritten = false;
    numOfReadings = 1;
    cellGain = readCell("Q2");
    cellTime = readCell("R2");

    gain = cellGain.toInt();
    gain /= 1000.0;
    
    time = cellTime.toInt();
  
    light.setGain(gain);
    light.setIntegTime(time);
    digitalWrite(npnLine, LOW);
    start = millis();
    luxVal = light.readLight();
    writeCell("S2", String(millis()-start));
    delay(1000);
    digitalWrite(npnLine, HIGH);
    writeCell("J5", String(luxVal-calibrationFactor));
    delay(delayVal);
    digitalWrite(analogLine, HIGH);
  }else {
    digitalWrite(analogLine, HIGH);
    averageWritten = false;
    numOfReadings = 1;
    cellGain = readCell("Q2");
    cellTime = readCell("R2");    
    gain = cellGain.toInt();
    gain /= 1000.0;
    
    time = cellTime.toInt();
  
    light.setGain(gain);
    light.setIntegTime(time);
    
    digitalWrite(npnLine, LOW);
    luxVal = light.readLight();
    delay(1000);
    digitalWrite(npnLine, HIGH);
    
    writeCell("I5", String(luxVal-calibrationFactor));
    delay(delayVal);
    digitalWrite(analogLine, HIGH);

  }
  writeCell(barcodeCell, "SV");
  delay(100);
  writeCell(barcodeCell_2, "A");
  delay(100);
  writeCell(actionCell, "D");
}

double calibrateSensor() {
  cellGain = readCell("Q2");
  cellTime = readCell("R2");
  
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
