int rightTick, leftTick;

void setup()
{
  Serial.begin(115200);
  attachInterrupt(5, updateLeftTick, RISING);
  attachInterrupt(4, updateRightTick, RISING);

  Serial.println("[INFO] Initialization Done.");
}

void loop()
{

  Serial.print("LeftTick ");
  Serial.println(leftTick);

  Serial.print("RightTick ");
  Serial.println(rightTick);
  
  delay(1000);
}

void updateLeftTick()
{
  leftTick++;
}

void updateRightTick()
{
  rightTick++;
}
