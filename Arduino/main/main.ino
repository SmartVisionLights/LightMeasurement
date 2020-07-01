String activate = "";
String numOfReadings = "";


void setup() {

  Serial.begin(9600); // the bigger number the better
  Serial.println("RESETTIMER"); //resets timer to 0
}

void loop() {
  activate = readCell("E2");
  numOfReadings = readCell("F2");
  
  if (activate.equals("Measuring")){
    Serial.println("CLEARDATA"); //clears up any data left from previous projects
    //writeCell("A1", "Values");
    writeCell("E2", "Measuring");
    writeCell("F2", String(numOfReadings));
    for (int i = 0; i < 100; i++) {
      String location = "A";
      location += (2 + i);
      if (i > numOfReadings.toInt()) {
        writeCell(location,"");
      }else{
        writeCell(location, String(random(0,100)));
      }
      delay(10);
    }
    writeCell("E2", "Measure");
  }

  delay(100);
}

void writeCell(String cell, String value) {
  Serial.print("CELL,SET," + cell + ",");
  Serial.println(value);
}

String readCell(String cell) {
  Serial.println("CELL,GET," + cell + ",");
  delay(100);
  String cellData = "";
  if (Serial.available() > 0) {
    cellData = Serial.readStringUntil('.');
    cellData.trim();
  }
  return cellData;
}
