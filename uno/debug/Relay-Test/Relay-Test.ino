#define R1 13
#define R2 14

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);
}

void loop() {
  Serial.println("Loop....");
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(R1, HIGH);
  digitalWrite(R2, LOW);
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(R1, LOW);
  digitalWrite(R2, HIGH);
  delay(1000);                       // wait for a second
}
