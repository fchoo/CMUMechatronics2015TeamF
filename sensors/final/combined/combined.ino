#define inputFSR 5   // FSR connected to analog input A5
#define inputPOT 3   // POT connected to analog input A3
#define inputIR 0    //  IR connected to analog input A0
#define loopCount 14

int potVal, fsrVal, irVal, irLinear;
unsigned int startByte3 = 0xBABE; // Decimal value of 47806
unsigned int startByte2 = 0xCAFE; // Decimal value of 51966
unsigned int startByte1 = 0xDEAD; // Decimal value of 57005

void setup() {
  pinMode(inputFSR, INPUT);
  pinMode(inputPOT, INPUT);
  pinMode(inputIR, INPUT);
  
  Serial.begin(9600);
}

void loop() {
  
  potVal = 0; fsrVal = 0; irVal = 0; // Resetting saved values
  
  // read and averages readings over a (100us*loopCount) time period
  for (int i = 0; i < loopCount; i++) {
    fsrVal += analogRead(inputFSR);
    potVal += analogRead(inputPOT);
    irVal += analogRead(inputIR);
  }

  fsrVal /= loopCount;
  potVal /= loopCount;
  irVal /= loopCount;

  irLinear = round(12343.85 * pow(irVal, -1.15)); // Linearizing eqn, accuracy +- 5%
  

  // Interlace the startBytes (0xDEAD, 0xCAFE, 0xBABE) with data bytes 
  Serial.write((unsigned byte*)&startByte1, 2);
  Serial.write((unsigned byte*)&fsrVal, 2);
  
  Serial.write((unsigned byte*)&startByte2, 2);
  Serial.write((unsigned byte*)&potVal, 2);
  
  Serial.write((unsigned byte*)&startByte3, 2);
  Serial.write((unsigned byte*)&irLinear, 2);
  
//  Serial.print("volt = ");
//  Serial.print(float(irVal)/1023*5);
//  Serial.print("\n");
//  Serial.print("linearized = ");
//  Serial.print(irLinear);
//  Serial.print("\n");
//  delay(1000);
}
