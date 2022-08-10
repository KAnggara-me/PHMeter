int pHSense = A0;
int samples = 12;
float adc_resolution = 4095.0;

void setup() {
  Serial.begin(9600);
  delay(100);
  Serial.println("cimpleo pH Sense");
}

float ph (float voltage) {
  return 7 + (2.5 - voltage);
}

void loop()
{
  int measurings = 0;
  for (int i = 0; i < samples; i++) {
    measurings += analogRead(pHSense);
    delay(10);
  }

  float voltage = (measurings / samples) * (3.3 / adc_resolution) ;
  Serial.print("pH= ");
  Serial.print(ph(voltage));
  Serial.print(" | RAW= ");
  Serial.println(voltage);
  delay(2000);
}
