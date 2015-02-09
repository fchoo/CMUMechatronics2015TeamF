int inputFSR = 5;   // FSR connected to analog input A5
int inputPOT = 3;   // POT connected to analog input A3
//int inputIR = 0;    //  IR connected to analog input A0

int sensorValue;

void setup() {
  pinMode(inputFSR, INPUT);
  pinMode(inputPOT, INPUT);
  //pinMode(inputIR, INPUT);
  
  Serial.begin(9600);
}

void loop() {
  delay(1000);
  
  // read and print FSR readings
  sensorValue = analogRead(inputFSR);
  Serial.print(sensorValue);
  Serial.print(",");

  // read and print POT readings
  sensorValue = analogRead(inputPOT);
  Serial.print(sensorValue);
  //Serial.print(",");
  Serial.print("\n");

  // read and print IR readings
  //sensorValue = analogRead(inputFSR);
  //Serial.println(sensorValue);  

}
