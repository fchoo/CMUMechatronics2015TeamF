float irVal, irDist;

void setup()
{
  Serial.begin(115200);
  pinMode(A7, INPUT);

  Serial.println("[INFO] Initialization Done.");
}

void loop()
{

  irVal = 0;

  for (int i = 0; i < 12; i++)
    irVal += analogRead(A7);

  irVal /= 12;
  irDist = 12343.85 * pow(irVal, -1.15); // Linearizing eqn, accuracy +- 5%
  Serial.print(" IRdist ");
  Serial.println(irDist);
}


