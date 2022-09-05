float calibration_value = 18.23 - 0.7; //21.34 - 0.7; //20.24 - 0.7;
int phval = 0;
int phPin = 39;

float ph_act;

void setup() {
  Serial.begin(115200);
  pinMode(phPin, INPUT);
}

void loop() {
  int avg = 0;
  for (int i = 1; i <= 10; i++) {
    avg += analogRead(phPin);
    delay(30);
  }

  float volt = (avg / 10) * (3.3 / 4095.0);

  ph_act = (-7.0 * volt + calibration_value) + 7.0;

  Serial.print(F("pH Val: "));
  Serial.print(ph_act, 1);
  Serial.print(F(" | Voltage: "));
  Serial.print(volt, 2);
  Serial.print(F(" | RAW: "));
  Serial.println(avg / 10);
  delay(1000);
}
