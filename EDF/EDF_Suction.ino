const int potpin = A2; // Analog pin for potentiometer.
const int EDF = 3; // Digital PWM pin for EDF.
int value = 0; // Set values you need to zero.

void setup()
{
  pinMode(3,OUTPUT); // Connect EDF pin.
  
  Serial.begin(9600);// Open serial port
  
  // Calibration for edf
  Serial.println("\r\n*Calibration begin*"); delay(200);
  // Initialize!
  analogWrite(EDF,0);
  Serial.println("Initializing to Zero..."); delay(500);
  // Engage!
  analogWrite(EDF,191);
  Serial.println("Engaging..."); delay(4000);
  Serial.println("*Calibration done*");
}


void loop()
{
  value = analogRead(potpin); // Read value of Potentiometer
  // Convert value from pot to a range of 180-255 to output to
  // the EDF. EDF operates between 190-255.
  value = map(value, 0, 1023, 180, 255); 
  
  // Extra spacing for more stopping range on pot.
  if(value<=189)
  {
    value=0;
  }
  
  analogWrite(EDF,value); // Write speed to EDF
  Serial.println(value);  
}
