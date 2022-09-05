int pHpin = 39;
float Voltage;

void setup() {
  Serial.begin(115200);
  pinMode(pHpin, INPUT);
}

void loop() {
  int measurings = 0;
  for (int i = 1; i <= 10; i++) {
    measurings += analogRead(pHpin);
    Serial.println(i);
    delay(10);
  }

  float voltage = (measurings / 10) * (3.3 / 4095.0);
  float raw = analogRead(pHpin);
  float phAir = ph(voltage);
  Serial.println("PH Saat ini => " + String(phAir) + " | V " + String(voltage) + " | RAW " + String(raw));
  delay(1000);
}

float ph (float voltage) {
  return 7 + (2.5 - voltage);
}
