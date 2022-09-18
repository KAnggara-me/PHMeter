#define pHpin 39

void setup() {
  Serial.begin(115200);
  pinMode(pHpin, INPUT);
}

void loop() {
  int measurings = 0;
  for (int i = 1; i <= 120; i++) {
    measurings += analogRead(pHpin);
    delay(25);
  }

  float voltage = (measurings / 120) * (3.3 / 4095.0);
  float raw = (measurings / 120);
  float phAir = (7 + (2.61 - voltage) / 0.18);
  Serial.println("PH Saat ini => " + String(phAir) + " | V " + String(voltage) + " | RAW " + String(raw));
  delay(1000);
}

//void loop() {
//  int measurings = 0;
//  for (int i = 1; i <= 120; i++) {
//    measurings += analogRead(pHpin);
//    Serial.println(analogRead(pHpin));
//    delay(25);
//  }
//
//  float raw = (measurings / 120);
//
//  Serial.println("Rata => " + String(raw));
//  delay(500);
//}
