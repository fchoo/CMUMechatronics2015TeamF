import processing.serial.*; // import serial library
import java.io.FileWriter;

// Initialize Global Variables
Serial arduinoPort;
PFont fontType;
int sensorReadings[];

// Canvas Data
int canvasH = 800;
int canvasW = 1500;

// Program Options
boolean dumpDataToFile = false; // Dump data to a .txt file in CSV
boolean enableFilter   = false;  // Enables a filter to smooth out data
int graphWidth = 1400;
int graphHeight = 200;

// Potentiometer Range
int potMax = 1023;
int potMin = 511;

// FSR Range
int fsrMax = 1023;
int fsrMin = 0;

// IR Range
int irMax = 0;
int irMin = 0;

// Create Arrays to store readings for filter
int[] IR_readings  = new int[graphWidth];
int[] POT_readings = new int[graphWidth];
int[] FSR_readings = new int[graphWidth];

// Create Graphs
cGraph IR_graph  = new cGraph(50, 550, graphWidth, graphHeight);
cGraph POT_graph = new cGraph(50, 300, graphWidth, graphHeight);
cGraph FSR_graph = new cGraph(50, 50, graphWidth, graphHeight);
 
void setup() {
  size(canvasW, canvasH, P2D);
  String portName = Serial.list()[0];
  arduinoPort = new Serial(this, portName, 9600);

  // wait until all 3 sensor inputs are transmitted
  arduinoPort.bufferUntil('\n');

}

void serialEvent(Serial arduinoPort) {
  String recString = arduinoPort.readStringUntil('\n');
  recString = trim(recString);

  if (recString != null) {
    sensorReadings = int(split(recString, ','));
    
    for (int sensorNum = 0; sensorNum < sensorReadings.length; sensorNum++) {
      if (sensorNum == 0) print("FSR reading: " + sensorReadings[0] + "\t ");
      else if (sensorNum == 1) print("POT reading: " + sensorReadings[1] + "\t");
      else if (sensorNum == 2) print("IR reading: " + sensorReadings[2] + "\t");
    }
    println();
    
    if (enableFilter) addDataWithFilter();
    else addDataWithoutFilter();
  }
  
  if (dumpDataToFile) {
    // write code to save data into file
  }
  
}

void addDataWithFilter() {
  // place holder for filter code
}

void addDataWithoutFilter() {
  
  // shift all values back by 1
  for(int i = 1; i < graphWidth; i++) { 
    IR_readings[i-1]  = IR_readings[i]; 
    POT_readings[i-1] = POT_readings[i]; 
    FSR_readings[i-1] = FSR_readings[i]; 
  } 
  
  // append new data to end of array
  for(int i = 0; i < sensorReadings.length; i++) { 
    if (i == 2)      IR_readings[graphWidth-1]  = sensorReadings[i]; 
    else if (i == 1) POT_readings[graphWidth-1] = sensorReadings[i]; 
    else if (i == 0) FSR_readings[graphWidth-1] = sensorReadings[i]; 
  } 
}

void draw() {
  
  // Draw the boxes for the 3 different graphs
  strokeWeight(0);
  IR_graph.drawGraphBox("Distance");
  FSR_graph.drawGraphBox("Force");
  POT_graph.drawGraphBox("Resistance");
  
  // Add in the graph labels
  fontType = loadFont("Calibri.vlw");
  textFont(fontType, 24);
  fill(0, 0, 0);
  textAlign(CENTER, BOTTOM);
  text("IR", 750, 50);
  text("POT", 750, 300);
  text("FSR", 750, 550);
  
  // Draw lines
  for (int i = 1; i < graphWidth; i++) {
    strokeWeight(2);
    if (POT_readings[i] != 0) {
      stroke(0, 255, 0);
      point(50+i, 500-float(POT_readings[i]-potMin)*graphHeight/(potMax-potMin));
    }
    if (FSR_readings[i] >= 10) {
      stroke(0, 0, 255);
      point(50+i, 750-float(FSR_readings[i]-fsrMin)*graphHeight/(fsrMax-fsrMin));
    }
    
  }
  
  stroke(0);
}
/*******************************************
 * Helper functions and Classes ************
 *******************************************/

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
    fontType = loadFont("Calibri.vlw");
    textFont(fontType, 16);
    fill(0, 0, 0);
    textAlign(CENTER, BOTTOM);
    text(axisLabel, m_gLeft, m_gTop-5);
    
    // add in x- and y-axis
    strokeWeight(2);
    fill(0, 0, 0);
    line(m_gLeft, m_gTop, m_gLeft, m_gBottom+5);        // draw y-axis
    line(m_gLeft-5, m_gBottom, m_gRight, m_gBottom);    // draw x-axis
    line(m_gLeft, m_gTop, m_gLeft-5, m_gTop+5);         // draw "arrows"
    line(m_gLeft, m_gTop, m_gLeft+5, m_gTop+5);         // draw "arrows"
    line(m_gRight, m_gBottom, m_gRight-5, m_gBottom+5); // draw "arrows"
    line(m_gRight, m_gBottom, m_gRight-5, m_gBottom-5); // draw "arrows"

  }

}
