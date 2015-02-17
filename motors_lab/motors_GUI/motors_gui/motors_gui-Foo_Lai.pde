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
          mode2 -- (1) FSR&stepper (2) pot&servo (3) IR->dc (4) switch -> something
*/
int mode = 0;
int mode2 = 0;

float stepperAngle = PI/2;
int stepperAngleInt = 360; // starts off at 0
float servoAngle = PI/2;
int servoAngleInt = 90;
float dcAngle = PI/2;
int dcAngleInt = 360;
int dcSpeed = 0;
int delay = 200;
int limit = delay;

// Handshake
int sensorHSnum = 2;
byte sensorDisplayHandShake[] = {(byte)0xAA,(byte)0xAA};
byte guiHS1, guiHS2, guiHS3, guiHS4;

/*************************************************************************
 ******* START OF MAIN FUNCTION ******************************************
 *************************************************************************/

void setup() {
  size(canvasW, canvasH, P2D);
  String portName = Serial.list()[0];
  arduinoPort = new Serial(this, portName, 9600);
}

void mouseClicked() {
    if ((455 <= mouseY) && (mouseY < 505) && (75 <= mouseX) && (mouseX < 175) && (mode == 2)){ // send cw
      print("CW : ");
      println(stepperAngleInt);
      guiHS1 = (byte)0x22;
      guiHS2 = (byte)0x22;
      guiHS3 = (byte)(stepperAngleInt >> 8);
      guiHS4 = (byte)(stepperAngleInt & 0xFF);
      sendGUIHS();
    }
    else if ((455 <= mouseY) && (mouseY < 505) && (190 <= mouseX) && (mouseX < 290) && (mode == 2)){ // send ccw
      print("CCW : ");
      println(stepperAngleInt);
      guiHS1 = (byte)0x33;
      guiHS2 = (byte)0x33;
      guiHS3 = (byte)(stepperAngleInt >> 8);
      guiHS4 = (byte)(stepperAngleInt & 0xFF);
      sendGUIHS();
    }
    else if ((450 <= mouseY) && (mouseY < 500) && (510 <= mouseX) && (mouseX < 610) && (mode == 2)){ // send ccw
      print("Servo: ");
      println(servoAngleInt);
      guiHS1 = (byte)0x44;
      guiHS2 = (byte)0x44;
      guiHS3 = (byte)(servoAngleInt >> 8);
      guiHS4 = (byte)(servoAngleInt & 0xFF);
      sendGUIHS();
    }
    else if ((600 <= mouseY) && (mouseY < 650) && (400 <= mouseX) && (mouseX < 500) && (mode == 2)){ // send cw
      dcSpeed = 0; // keep this
      print("CW DC Degree: ");
      println(dcAngleInt);
      guiHS1 = (byte)0x55;
      guiHS2 = (byte)0x55;
      guiHS3 = (byte)(dcAngleInt >> 8);
      guiHS4 = (byte)(dcAngleInt & 0xFF);
      sendGUIHS();
    }
    else if ((600 <= mouseY) && (mouseY < 650) && (550 <= mouseX) && (mouseX < 650) && (mode == 2)){ // send ccw
      dcSpeed = 0; // keep this
      print("CCW DC Degree: ");
      println(dcAngleInt);
      guiHS1 = (byte)0x66;
      guiHS2 = (byte)0x66;
      guiHS3 = (byte)(dcAngleInt >> 8);
      guiHS4 = (byte)(dcAngleInt & 0xFF);
      sendGUIHS();
    }
    else if ((725 <= mouseY) && (mouseY < 775) && (475 <= mouseX) && (mouseX < 575) && (mode == 2)){ // send speed
      dcAngleInt = 360; // keep this
      dcAngle = PI/2; // keep this too
      print("DC Speed: ");
      println(dcSpeed);
      if (dcSpeed >= 0) {
        guiHS1 = (byte)0x77;
        guiHS2 = (byte)0x77;
        guiHS3 = (byte)(dcSpeed >> 8);
        guiHS4 = (byte)(dcSpeed & 0xFF);
      }
      else {
        guiHS1 = (byte)0x88;
        guiHS2 = (byte)0x88;
        guiHS3 = (byte)(-(dcSpeed) >> 8);
        guiHS4 = (byte)(-(dcSpeed) & 0xFF);
      }
      sendGUIHS();
    }
}

void sendGUIHS() {
    arduinoPort.write(guiHS1);
    arduinoPort.write(guiHS2);
    arduinoPort.write(guiHS3);
    arduinoPort.write(guiHS4);
}

void draw() {

  // Mode selection - default mode = 0
  if (mousePressed) {
    if ((50 <= mouseY) && (mouseY <= 100) && (50 <= mouseX) && (mouseX <= 325)) {
      if (mode != 1) clearSensorData();
      mode = 1; mode2 = 0;
    }
    else if ((50 <= mouseY) && (mouseY <= 100) && (425 <= mouseX) && (mouseX <= 700)) {
      mode = 3; mode2 = 0;
    }
    else if ((150 <= mouseY) && (mouseY <= 200) && (50 <= mouseX) && (mouseX <= 700)) {
      mode = 2; mode2 = 0;
      arduinoPort.clear();
      arduinoPort.stop();
      String portName = Serial.list()[0];
      arduinoPort = new Serial(this, portName, 9600);
    }
    else if ((250 <= mouseY) && (mouseY <= 300) && (150 <= mouseX) && (mouseX <= 300) && (mode == 3)) {
      mode2 = 1;
    }
    else if ((450 <= mouseY) && (mouseY <= 500) && (150 <= mouseX) && (mouseX <= 300) && (mode == 3)) {
      mode2 = 2;
    }
    else if ((250 <= mouseY) && (mouseY <= 300) && (450 <= mouseX) && (mouseX <= 600) && (mode == 3)) {
      mode2 = 3;
    }
    else if ((450 <= mouseY) && (mouseY <= 500) && (450 <= mouseX) && (mouseX <= 600) && (mode == 3)) {
      mode2 = 4;
    }

    // CONTROL STEPPER MOTOR
    else if ((340 <= mouseY) && (mouseY < 390) && (182 <= mouseX) && (mouseX <= 235) && (mode == 2)) { // quadrant 1
      stepperAngle = atan(float(390-mouseY)/(mouseX-182));
      stepperAngleInt = 90 - round(degrees(stepperAngle));
    }
    else if ((340 <= mouseY) && (mouseY < 390) && (130 <= mouseX) && (mouseX < 182) && (mode == 2)) { // quadrant 2
      stepperAngle = atan(float(390-mouseY)/(182-mouseX));
      stepperAngleInt = 270 + round(degrees(stepperAngle));
    }
    else if ((390 <= mouseY) && (mouseY < 445) && (130 <= mouseX) && (mouseX < 182) && (mode == 2)) { // quadrant 3
      stepperAngle = atan(float(mouseY-390)/(182-mouseX));
      stepperAngleInt = 270 - round(degrees(stepperAngle));
    }
    else if ((390 <= mouseY) && (mouseY < 445) && (182 <= mouseX) && (mouseX < 235) && (mode == 2)) { // quadrant 4
      stepperAngle = atan(float(mouseY-390)/(mouseX-182));
      stepperAngleInt = 90 + round(degrees(stepperAngle));
    }

    // SERVO MOTOR CONTROL
    else if ((330 <= mouseY) && (mouseY < 445) && (460 <= mouseX) && (mouseX < 560) && (mode == 2)) { // left side
      servoAngle = atan(float(440-mouseY)/(560-mouseX));
      servoAngleInt = round(degrees(servoAngle));
      if (servoAngleInt < 0) servoAngleInt = 0;
    }
    else if ((330 <= mouseY) && (mouseY < 445) && (560 <= mouseX) && (mouseX < 660) && (mode == 2)) { // right side
      servoAngle = atan(float(440-mouseY)/(mouseX-560));
      servoAngleInt = 180-round(degrees(servoAngle));
      if (servoAngleInt > 180) servoAngleInt = 180;
    }

    // DC MOTOR
    else if ((570 <= mouseY) && (mouseY < 625) && (250 <= mouseX) && (mouseX <= 310) && (mode == 2)) { // quadrant 1
      dcAngle = atan(float(625-mouseY)/(mouseX-250));
      dcAngleInt = 90 - round(degrees(dcAngle));
    }
    else if ((570 <= mouseY) && (mouseY < 625) && (195 <= mouseX) && (mouseX < 250) && (mode == 2)) { // quadrant 2
      dcAngle = atan(float(625-mouseY)/(250-mouseX));
      dcAngleInt = 270 + round(degrees(dcAngle));
    }
    else if ((625 <= mouseY) && (mouseY < 680) && (195 <= mouseX) && (mouseX < 250) && (mode == 2)) { // quadrant 3
      dcAngle = atan(float(mouseY-625)/(250-mouseX));
      dcAngleInt = 270 - round(degrees(dcAngle));
    }
    else if ((625 <= mouseY) && (mouseY < 680) && (250 <= mouseX) && (mouseX < 310) && (mode == 2)) { // quadrant 4
      dcAngle = atan(float(mouseY-625)/(mouseX-250));
      dcAngleInt = 90 + round(degrees(dcAngle));
    }
    else if ((725 <= mouseY) && (mouseY < 775) && (145 <= mouseX) && (mouseX < 350) && (mode == 2)) { // quadrant 4
      dcSpeed = round(float(mouseX-250)/100*255);
      if (dcSpeed > 255) dcSpeed = 255;
      if (dcSpeed < -255) dcSpeed = -255;
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
  text("Control motors with Computer", 375, 175);

  // Draw mode 3 button (control motor with sensors)
  strokeWeight(2);
  rectMode(CORNERS);
  if (mode != 3) fill(255, 255, 255);
  else fill(0, 255, 0, 10);
  rect(425, 50 , 700, 100);
  fill(0);
  textAlign(CENTER, CENTER);
  textSize(16);
  text("Control motors with sensors", 558, 75);

  // Default empty handshake
  if (mode == 0) {
    arduinoPort.write(0xFF);
    arduinoPort.write(0xFF);
  }
  if (mode == 3) { // control motor with sensors
    strokeWeight(2);
    rectMode(CENTER);
    if (mode2 == 1) fill(0, 255, 0, 10);
    else fill(255);
    rect(225, 275, 150, 50);
    if (mode2 == 2) fill(0, 255, 0, 10);
    else fill(255);
    rect(225, 475, 150, 50);
    if (mode2 == 3) fill(0, 255, 0, 10);
    else fill(255);
    rect(525, 275, 150, 50);
    if (mode2 == 4) fill(0, 255, 0, 10);
    else fill(255);
    rect(525, 475, 150, 50);
    fill(0);
    textAlign(CENTER, CENTER);
    text("FSR->Stepper", 225, 275);
    text("POT->Servo", 225, 475);
    text("IR->DC", 525, 275);
    text("Switch->Stepper", 525, 475);
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
    text(stepperAngleInt, 182, 390);
    fill(255, 0, 0);
    ellipseMode(CENTER);
    if ((90 <= stepperAngleInt) && (stepperAngleInt < 180))
      ellipse(182+50*cos(stepperAngle), 390+50*sin(stepperAngle), 10, 10);
    else if ((0 <= stepperAngleInt) && (stepperAngleInt < 90))
      ellipse(182+50*cos(stepperAngle), 390-50*sin(stepperAngle), 10, 10);
    else if ((180 <= stepperAngleInt) && (stepperAngleInt < 270))
      ellipse(182-50*cos(stepperAngle), 390+50*sin(stepperAngle), 10, 10);
    else if ((270 <= stepperAngleInt) && (stepperAngleInt <= 360))
      ellipse(182-50*cos(stepperAngle), 390-50*sin(stepperAngle), 10, 10);

    // Draw Servo controls
    fill(0);
    text("Servo", 560, 300);
    text("0", 447, 440);
    text("180", 680, 440);
    line(535, 310, 585, 310); // underline
    fill(222, 201, 201);
    strokeWeight(4);
    arc(560, 440, 200, 200, -PI, 0);
    fill(0);
    text(servoAngleInt, 560, 415);
    line(457, 440, 463, 440); line(657, 440, 663, 440);
    fill(255, 0, 0);
    strokeWeight(2);
    if ((0 <= servoAngleInt) && (servoAngleInt < 90))
      ellipse(560-100*cos(servoAngle), 440-100*sin(servoAngle), 10, 10);
    else if ((90 <= servoAngleInt) && (servoAngleInt <= 180))
      ellipse(560+100*cos(servoAngle), 440-100*sin(servoAngle), 10, 10);
    fill(255);
    rect(560, 475, 100, 50);
    fill(0);
    text("GO", 560, 475);

    // Draw DC controls part 1 -> angle
    fill(0);
    textAlign(CENTER, CENTER);
    text("DC Motor", 375, 550);
    strokeWeight(2);
    line(340, 565, 410, 565); // underline
    fill(255);
    ellipseMode(CENTER);
    ellipse(250, 625, 100, 100);
    fill(0);
    text(dcAngleInt, 250, 625);
    fill(255, 0, 0);
    ellipseMode(CENTER);
    if ((90 <= dcAngleInt) && (dcAngleInt < 180))
      ellipse(250+50*cos(dcAngle), 625+50*sin(dcAngle), 10, 10);
    else if ((0 <= dcAngleInt) && (dcAngleInt < 90))
      ellipse(250+50*cos(dcAngle), 625-50*sin(dcAngle), 10, 10);
    else if ((180 <= dcAngleInt) && (dcAngleInt < 270))
      ellipse(250-50*cos(dcAngle), 625+50*sin(dcAngle), 10, 10);
    else if ((270 <= dcAngleInt) && (dcAngleInt <= 360))
      ellipse(250-50*cos(dcAngle), 625-50*sin(dcAngle), 10, 10);
    fill(255);
    rect(450, 625, 100, 50);
    rect(600, 625, 100, 50);
    fill(0);
    text("CW", 450, 625);
    text("CCW", 600, 625);

    // Draw DC controls part 2 -> speed
    text(dcSpeed, 250, 780);
    strokeWeight(4);
    line(150, 750, 350, 750);
    fill(255);
    strokeWeight(2);
    rect(250+round(float(dcSpeed)/255*100), 750, 25, 40);
    strokeWeight(2);
    rect(525, 750, 100, 50);
    fill(0);
    text("GO", 525, 750);
  }

  // Mode 1 - Display sensor readings
  if (mode == 1) {
    for (int i=0; i< sensorHSnum; i++) {
      arduinoPort.write(0x11);
      arduinoPort.write(0x11);
    }

    // Ensure all transmitted data is read
    while (arduinoPort.available() >= 4*numSensors) {
      // 2 bytes for start bits, 2 bytes for data bits
      processSerialData(0xDE, 0xAD, "FSR");
      processSerialData(0xCA, 0xFE, "POT");
      processSerialData(0xBA, 0xBE, "IR");

      // Tell Arduino to send next set of data
      arduinoPort.write(0x11);
      arduinoPort.write(0x11);
    }
  }

  // Mode 3 - Control motors using sensor
  if (mode == 3) {
    if (mode2 == 1) {
      arduinoPort.write(0x99);
      arduinoPort.write(0x99);
    }
    else if (mode2 == 2) {
      arduinoPort.write(0xAA);
      arduinoPort.write(0xAA);
    }
    else if (mode2 == 3) {
      arduinoPort.write(0xBB);
      arduinoPort.write(0xBB);
    }
    else if (mode2 == 4) {
      arduinoPort.write(0xCC);
      arduinoPort.write(0xCC);
    }
    
        // Ensure all transmitted data is read
    while (arduinoPort.available() >= 4*numSensors) {
      // 2 bytes for start bits, 2 bytes for data bits
      processSerialData(0xDE, 0xAD, "FSR");
      processSerialData(0xCA, 0xFE, "POT");
      processSerialData(0xBA, 0xBE, "IR");
    }
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
  if (mode == 1 || mode == 3) {
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
    if (mode == 1 || mode ==3) addData(sensorName, sensorReadings);
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

