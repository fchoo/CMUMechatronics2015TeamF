import processing.serial.*; // import serial library
import java.io.FileWriter;

// Initialize Global Variables
Serial arduinoPort;
int sensorReadings;
int numSensors = 3;
byte[] byteBuf = new byte[2];

// Graph index for each sensor
int potIndex = 0;
int fsrIndex = 0;
int irIndex  = 0;

// Canvas Data
int canvasH = 800;
int canvasW = 1500;

// Program Options
boolean dumpDataToFile = false; // Dump data to a .txt file in CSV
boolean pause = false;
int graphWidth = 600;
int graphHeight = 200;

// Potentiometer Range
int potMax = 1023;
int potMin = 511;
String potMaxValue = "10"; // max pot resistance of 10k ohms

// FSR Range
int fsrMax = 1023;
int fsrMin = 0;
String fsrMaxValue = "10"; // max force value of 10kg

// IR Range
int irMax = 80;
int irMin = 10;
String irMaxValue = "80"; // max ir distance of 80cm

// Create Arrays to store readings for filter
int[] IR_readings  = new int[graphWidth];
int[] POT_readings = new int[graphWidth];
int[] FSR_readings = new int[graphWidth];

// Create Graphs
cGraph IR_graph  = new cGraph(800, 550, graphWidth, graphHeight);
cGraph POT_graph = new cGraph(800, 300, graphWidth, graphHeight);
cGraph FSR_graph = new cGraph(800,  50, graphWidth, graphHeight);

// Strings
String irString  = "Distance (cm)";
String fsrString = "Force (kg)";
String potString = "Resistance (k Ohms)";

/* Modes:
   (0) - Default, doesn't do anything
   (1) - Display sensor readings
   (2) - Control motors using GUI 
   (3) - Control motors using sensors
          mode2 -- (1) FSR&stepper (2) pot&servo (3) IR->dc
*/
int mode = 0;
int mode2 = 0;

// Handshake
int sensorHSnum = 2;
byte sensorDisplayHandShake[] = {(byte)0xAA,(byte)0xAA};

/*************************************************************************
 ******* START OF MAIN FUNCTION ******************************************
 *************************************************************************/

void setup() {
  size(canvasW, canvasH, P2D);
  String portName = Serial.list()[0];
  arduinoPort = new Serial(this, portName, 9600);
}

void draw() {
  
  // Mode selection - default mode = 0
  if (mousePressed) {
    if ((50 <= mouseY) && (mouseY <= 100) && (50 <= mouseX) && (mouseX <= 325)) {
      if (mode != 1) clearSensorData();
      mode = 1; mode2 = 0;
    }
    else if ((50 <= mouseY) && (mouseY <= 100) && (425 <= mouseX) && (mouseX <= 700)){
      mode = 3; mode2 = 1;
    }
    else if ((150 <= mouseY) && (mouseY <= 200) && (50 <= mouseX) && (mouseX <= 700)){
      mode = 2; mode2 = 0;
    }
    else if ((275 <= mouseY) && (mouseY <= 325) && (50 <= mouseX) && (mouseX <= 200) && (mode == 3)){
      mode2 = 1;
    }
    else if ((275 <= mouseY) && (mouseY <= 325) && (300 <= mouseX) && (mouseX <= 450) && (mode == 3)){
      mode2 = 2;
    }
    else if ((275 <= mouseY) && (mouseY <= 325) && (550 <= mouseX) && (mouseX <= 700) && (mode == 3)){
      mode2 = 3;
    }
  }

  // Clear current canvas
  background(#DEC9C9);

  // Draw mode 1 button (read from sensors)
  strokeWeight(2);
  rectMode(CORNERS);
  if (mode != 1) fill(255, 255, 255);
  else fill(0, 255, 0, 10);
  rect(50, 50, 325, 100);
  fill(0);
  textAlign(CENTER, CENTER);
  text("Read values from sensors", 183, 75);

  // Draw mode 2 button (control motor with GUI)
  strokeWeight(2);
  rectMode(CORNERS);
  if (mode != 2) fill(255, 255, 255);
  else fill(0, 255, 0, 10);
  rect(50, 150, 700, 200);
  fill(0);
  textAlign(CENTER, CENTER);
  textSize(16);
  text("Control motors with sensor", 375, 175);
  
  // Draw mode 3 button (control motor with sensors)
  strokeWeight(2);
  rectMode(CORNERS);
  if (mode != 3) fill(255, 255, 255);
  else fill(0, 255, 0, 10);
  rect(425, 50 , 700, 100);
  fill(0);
  textAlign(CENTER, CENTER);
  textSize(16);
  text("Control motors with computer", 558, 75);
  
  if (mode == 3) { // control motor with sensors
    strokeWeight(2);
    rectMode(CENTER);
    if (mode2 == 1) fill(0, 255, 0, 10);
    else fill(255);
    rect(125, 300, 150, 50);
    if (mode2 == 2) fill(0, 255, 0, 10);
    else fill(255);
    rect(375, 300, 150, 50);
    if (mode2 == 3) fill(0, 255, 0, 10);
    else fill(255);
    rect(625, 300, 150, 50);
    fill(0);
    textAlign(CENTER, CENTER);
    text("FSR->Stepper", 125, 300);
    text("POT->Servo", 375, 300);
    text("IR->DC", 625, 300);
  }  
  
  if (mode == 2) { // control motor with computer
    // Draw Servo Controls
    fill(0);
    textAlign(CENTER, CENTER);
    text("Stepper", 183, 300);
    strokeWeight(2);
    line(150, 310, 215, 310); // underline
    fill(255);
    ellipseMode(CENTER);
    ellipse(182, 390, 100, 100);
    rectMode(CENTER);
    rect(125, 480, 100, 50);
    rect(240, 480, 100, 50);
    fill(0);
    textAlign(CENTER, CENTER);
    text("CW", 125, 480);
    text("CCW", 240, 480);
  }
  
  // Mode 1 - Display sensor readings
  if (mode == 1) {
    if (sensorHSnum > 0) {
      arduinoPort.write(sensorDisplayHandShake[0]);
      arduinoPort.write(sensorDisplayHandShake[1]);
      sensorHSnum--;
    }
    
    // Ensure all transmitted data is read
    while (arduinoPort.available() >= 4*numSensors) {
      // 2 bytes for start bits, 2 bytes for data bits
      processSerialData(0xDE, 0xAD, "FSR");
      processSerialData(0xCA, 0xFE, "POT");
      processSerialData(0xBA, 0xBE, "IR");
      
      // Tell Arduino to send next set of data
      arduinoPort.write(sensorDisplayHandShake[0]);
      arduinoPort.write(sensorDisplayHandShake[1]);  
    }
  }
  
  // Mode 3 - Control motors using sensor
  if (mode == 3) {
    println(mode2);
    if (mode2 == 1) {
      arduinoPort.write(0xCC);
      arduinoPort.write(0xAA);  
    }
    else if (mode2 == 2) {
      arduinoPort.write(0xCC);
      arduinoPort.write(0xBB);  
    }
    else if (mode2 == 3) {
      arduinoPort.write(0xCC);
      arduinoPort.write(0xCC);  
    }
    while (arduinoPort.available() <2);
    println(hex(arduinoPort.read()));    println(hex(arduinoPort.read()));
  }
  
  
  
  // Draw the boxes for the 3 different graphs
  strokeWeight(0);
  IR_graph.drawGraphBox(irString);
  FSR_graph.drawGraphBox(fsrString);
  POT_graph.drawGraphBox(potString);

  // Add in the graph labels
  fill(0, 0, 0);
  textAlign(CENTER, BOTTOM);
  text("IR", 1125, 50);
  text("POT", 1125, 300);
  text("FSR", 1125, 550);

  // Draw graphs only when sensor mode is chosen
  if (mode == 1) {
    drawPOTgraph();
    drawIRgraph();
    drawFSRgraph();
  }  

  stroke(0);
}

/*************************************************************************
 ******* HELPER FUNCTIONS AND CLASS DECLARATIONS *************************
 *************************************************************************/

// This class takes the data and graph it
class cGraph
{
  float m_gWidth, m_gHeight;
  float m_gLeft, m_gBottom, m_gRight, m_gTop;

  cGraph(float x, float y, float w, float h)
  {
    m_gWidth     = w;
    m_gHeight    = h;
    m_gLeft      = x;
    m_gBottom    = canvasH - y;
    m_gRight     = x + w;
    m_gTop       = canvasH - y - h;
  }

  void drawGraphBox(String axisLabel)
  {
    // draw the graph area
    strokeWeight(0);
    rectMode(CORNERS);
    fill(255, 255, 255);
    rect(m_gLeft, m_gBottom , m_gRight, m_gTop);

    // add in y-axis label
    fill(0, 0, 0);
    textAlign(CENTER, BOTTOM);
    if (axisLabel != potString) text(axisLabel, m_gLeft, m_gTop-5);
    else text(axisLabel, m_gLeft+15, m_gTop-5);

    // add in x- and y-axis
    strokeWeight(2);
    fill(0, 0, 0);
    line(m_gLeft, m_gTop, m_gLeft, m_gBottom+5);        // draw y-axis
    line(m_gLeft-5, m_gBottom, m_gRight, m_gBottom);    // draw x-axis
    line(m_gLeft, m_gTop, m_gLeft-5, m_gTop+5);         // draw "arrows"
    line(m_gLeft, m_gTop, m_gLeft+5, m_gTop+5);         // draw "arrows"
    line(m_gRight, m_gBottom, m_gRight-5, m_gBottom+5); // draw "arrows"
    line(m_gRight, m_gBottom, m_gRight-5, m_gBottom-5); // draw "arrows"

    // "0" label
    if (axisLabel == irString) {
      textAlign(RIGHT, CENTER);
      text("10", m_gLeft-10, m_gBottom);
    }
    else {
      textAlign(CENTER, TOP);
      text("0", m_gLeft-10, m_gBottom);
    }
    
    // Max Value label
    textAlign(RIGHT, CENTER);
    if      (axisLabel == irString)  text(irMaxValue, m_gLeft-10, m_gTop);
    else if (axisLabel == fsrString) text(fsrMaxValue, m_gLeft-10, m_gTop);
    else if (axisLabel == potString) text(potMaxValue, m_gLeft-10, m_gTop);

    // Distance reader
    if (axisLabel == irString) {
      fill(0,0,0);
      strokeWeight(1);
      line(m_gLeft, m_gBottom+(m_gTop-m_gBottom)/7*1, m_gRight, m_gBottom+(m_gTop-m_gBottom)/7*1);    // draw x-axis
      textAlign(RIGHT, CENTER);
      text("20", m_gLeft-10, m_gBottom+(m_gTop-m_gBottom)/7*1);

      line(m_gLeft, m_gBottom+(m_gTop-m_gBottom)/7*2, m_gRight, m_gBottom+(m_gTop-m_gBottom)/7*2);    // draw x-axis
      textAlign(RIGHT, CENTER);
      text("30", m_gLeft-10, m_gBottom+(m_gTop-m_gBottom)/7*2);

      line(m_gLeft, m_gBottom+(m_gTop-m_gBottom)/7*4, m_gRight, m_gBottom+(m_gTop-m_gBottom)/7*4);    // draw x-axis
      textAlign(RIGHT, CENTER);
      text("50", m_gLeft-10, m_gBottom+(m_gTop-m_gBottom)/7*4);

      line(m_gLeft, m_gBottom+(m_gTop-m_gBottom)/7*5, m_gRight, m_gBottom+(m_gTop-m_gBottom)/7*5);    // draw x-axis
      textAlign(RIGHT, CENTER);
      text("60", m_gLeft-10, m_gBottom+(m_gTop-m_gBottom)/7*5);

      line(m_gLeft, m_gBottom+(m_gTop-m_gBottom)/7*3, m_gRight, m_gBottom+(m_gTop-m_gBottom)/7*3);    // draw x-axis
      textAlign(RIGHT, CENTER);
      text("40", m_gLeft-10, m_gBottom+(m_gTop-m_gBottom)/7*3);

  }

    // "Time label"
    textAlign(LEFT, CENTER);
    text("Time", m_gRight+5, m_gBottom);
  }

}

void drawIRgraph() {
  for (int i = irIndex; i < graphWidth; i++) {
      if (IR_readings[i] > 0) {
        stroke(255, 0, 0);
        point(i-irIndex+800, 250-float(IR_readings[i]-irMin)*graphHeight/(irMax-irMin));
      }
    }
    for (int i = 0; i < irIndex; i++) {
      if (IR_readings[i] > 0) {
        stroke(255, 0, 0);
        point(graphWidth-irIndex+i+800, 250-float(IR_readings[i]-irMin)*graphHeight/(irMax-irMin));
      }
    }
}

void drawFSRgraph() {
  for (int i = fsrIndex; i < graphWidth; i++) {
    if (FSR_readings[i] >= 10) {
      if ((FSR_readings[i] <= 600) && (FSR_readings[i] > 300)) stroke(93, 152, 86);
      else stroke(255, 0, 0);
      point(i-fsrIndex+800, 750-float(FSR_readings[i]-fsrMin)*graphHeight/(fsrMax-fsrMin));
    }
  }
  for (int i = 0; i < fsrIndex; i++) {
    if (FSR_readings[i] >= 10) {
      if ((FSR_readings[i] <= 600) && (FSR_readings[i] > 300)) stroke(93, 152, 86);
      else stroke(255, 0, 0);
      point(graphWidth-fsrIndex+i+800, 750-float(FSR_readings[i]-fsrMin)*graphHeight/(fsrMax-fsrMin));
    }
  }
}

void drawPOTgraph() {
  for (int i = potIndex; i < graphWidth; i++) {
    strokeWeight(2);
    if (POT_readings[i] != 0) {
      stroke(0, 0, 255);
      point(i-potIndex+800, 300+float(POT_readings[i]-potMin)*graphHeight/(potMax-potMin));
    }
  }
  for (int i = 0; i < potIndex; i++) {
    strokeWeight(2);
    if (POT_readings[i] != 0) {
      stroke(0, 0, 255);
      point(graphWidth-potIndex+i+800, 300+float(POT_readings[i]-potMin)*graphHeight/(potMax-potMin));
    }
  }
}

void processSerialData(int startByte2, int startByte1, String sensorName) {
  int curMatchPos = 0;
  int recByte = 0;
  int[] startBitBuf = new int[2];

  startBitBuf[0] = startByte1;
  startBitBuf[1] = startByte2;   // Combined start-bits == 0x'startByte2''startByte1'

  while (arduinoPort.available() < 2); // Loop until we have enough bytes
  recByte = arduinoPort.read();

  // start hand-shake protocol
  while(curMatchPos < 2) {

    if (recByte == startBitBuf[curMatchPos]) {
      // start bits match
      curMatchPos++;

      if (curMatchPos == 2) // 2 here refers to number of start bytes
        break;

      while (arduinoPort.available() < 2) {
      };       // Wait til there are sufficient bytes in serial buffer
      recByte = arduinoPort.read();
    }
    else {
      // start bits do not match; try again
      if (curMatchPos == 0) {
        while (arduinoPort.available() < 2); // Wait til there are sufficient bytes in serial buffer
        recByte = arduinoPort.read();
      }
      else {
        curMatchPos = 0;
      }
    }
  }

  while (arduinoPort.available() < 2);
  // Wait til there are sufficient bytes in serial buffer
  // This reads in one set of data
  {
    arduinoPort.readBytes(byteBuf);
    sensorReadings = ((int)(byteBuf[1]&0xFF) << 8) + ((int)(byteBuf[0]&0xFF) << 0);
    if (mode == 1) addData(sensorName, sensorReadings);
  }
}

void clearSensorData() {
  for (int i = 0; i < graphWidth; i++) {
    IR_readings[i] = 0;
    FSR_readings[i] = 0;
    POT_readings[i] = 0;
  }
}

void addData(String sensorName, int sensorReadings) {
  if (sensorName == "FSR") {
    FSR_readings[fsrIndex] = sensorReadings;
    fsrIndex++;
    if (fsrIndex == graphWidth) fsrIndex = 0;
  }
  else if (sensorName == "POT") {
    //sensorReadings = potMax-sensorReadings;
    POT_readings[potIndex] = sensorReadings;
    potIndex++;
    if (potIndex == graphWidth) potIndex = 0;
  }
  else if (sensorName == "IR") {
    if ((sensorReadings <= irMax) && (sensorReadings >= irMin))
      IR_readings[irIndex] = sensorReadings;
    //println(sensorReadings);
    irIndex++;
    if (irIndex == graphWidth) irIndex = 0;
  }
  else println("Error S0S: Sensor Not Found");
}

