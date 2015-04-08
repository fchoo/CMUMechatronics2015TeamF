// LEDs
#define PIN_RED2 53
#define PIN_RED1 51
#define PIN_BLUE 47
#define PIN_GREEN2 49
#define PIN_GREEN1 45

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(PIN_RED1, OUTPUT);
  pinMode(PIN_RED2, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  pinMode(PIN_GREEN1, OUTPUT);
  pinMode(PIN_GREEN2, OUTPUT);
  digitalWrite(PIN_RED1, LOW);
  digitalWrite(PIN_RED2, LOW);
  digitalWrite(PIN_BLUE, LOW);
  digitalWrite(PIN_GREEN1, LOW);
  digitalWrite(PIN_GREEN2, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(PIN_GREEN1, HIGH);
  delay(1000);
  digitalWrite(PIN_GREEN2, HIGH);
  delay(1000);
  digitalWrite(PIN_BLUE, HIGH);
  delay(1000);
  digitalWrite(PIN_RED1, HIGH);
  delay(1000);
  digitalWrite(PIN_RED2, HIGH);
  delay(1000);
  digitalWrite(PIN_RED1, LOW);
  digitalWrite(PIN_RED2, LOW);
  digitalWrite(PIN_BLUE, LOW);
  digitalWrite(PIN_GREEN1, LOW);
  digitalWrite(PIN_GREEN2, LOW);
  delay(1000);
}




