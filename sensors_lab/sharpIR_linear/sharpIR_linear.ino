#define IR_APIN 0

void setup()
{
  Serial.begin(9600);
}

// If this is defined it prints out the FPS that we can send a
// complete set of data over the serial port.
//#define CHECK_FPS

void loop()
{
  int val=0;
  int dist=0;
  unsigned int startTag = 0xDEAD;  // Analog port maxes at 1023 so this is a safe termination value
  int loopCount;

#ifdef CHECK_FPS  
  unsigned long startTime, endTime;
  startTime = millis();
#endif
  
  // Can't do more than 64 loops or could overflow the 16 bit ints
  // This just averages together as many sensor reads as we can in
  // order to reduce sensor noise.  Might want to introduce add
  // a smarter filter her in the future.
  loopCount = 12;  // 12 gives a little over 100 FPS
  for(int i = 0; i< loopCount; ++i)
  {
    // It takes 100 us (0.0001 s) to read an analog input
    val += analogRead(IR_APIN);
  }
  val /= loopCount;
//  dist = (6787/(val-3))-4;
  float distance = 12343.85 * pow(val, -1.15); // Linearizing eqn, accuracy +- 5%
  Serial.print("Dist: ");
  Serial.println(distance);
  delay(10);
  
#ifdef CHECK_FPS  
  endTime = millis();
  Serial.print(" - FPS: ");
  Serial.println(1.f / (endTime-startTime) * 1000);
#endif
}

