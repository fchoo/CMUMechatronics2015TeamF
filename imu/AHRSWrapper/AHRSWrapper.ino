/**
*
* An example of program to utilizes the AHRS. Key functions are
*
* AHRS_Init() - Initialize the AHRS
* Read_AHRS() - Update roll, pitch, yaw angles in AHRS
* Get_Roll()  - Get Roll angle in AHRS
* Get_Pitch()  - Get Pitch angle in AHRS
* Get_Yaw()  - Get Yaw angle in AHRS
*
* This is based on MinIMU-9-Arduino-AHRS by Pololu:
* https://github.com/pololu/minimu-9-ahrs-arduino
*
**/

#define STATUS_LED 13

void setup()
{
  Serial.begin(115200);
  pinMode (STATUS_LED,OUTPUT);  // Status LED
  digitalWrite(STATUS_LED,LOW); // OFF - Calibrating

  AHRS_Init();

  digitalWrite(STATUS_LED,HIGH); // ON - Calibration done
}

void loop() //Main Loop
{
  Read_AHRS();
  Serial.print("ANG:");
  Serial.print(Get_Roll());
  Serial.print(",");
  Serial.print(Get_Pitch());
  Serial.print(",");
  Serial.print(Get_Yaw());
}
