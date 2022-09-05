//Include the library
#include <MQUnifiedsensor.h>
#define Pin      (36)
#define CleanAir (3.6)
MQUnifiedsensor MQ("ESP-32", 3.3, 12, Pin, "MQ-135");

void setup() {
  Serial.begin(115200);
  Serial.println(F("Init MQ-135 please wait..."));
  delay(20000);
  MQInit();
}

// Get Board R0
float getR0() {
  int sample = 10;
  float R0 = 0;
  for (int i = 1; i <= sample; i ++) {
    MQ.update();
    R0 += MQ.calibrate(CleanAir);
    Serial.print(".");
  }
  return R0 / (sample * 1);
}

void MQInit() {
  MQ.init();
  MQ.setRegressionMethod(1);
  Serial.println(F("Calibrating please wait."));
  float R0 = getR0();
  MQ.setR0(R0);
  Serial.println(F("\nDone!."));
  if (isinf(R0)) {
    Serial.println(F("Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply"));
    while (1);
  }
  if (R0 == 0) {
    Serial.println(F("Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply"));
    while (1);
  }
}

void loop() {
  MQ.update();
  MQ.setA(102.2 ); MQ.setB(-2.473); //NH4
  float NH4 = MQ.readSensor();
  Serial.print(F("NH4 => ")); Serial.print(NH4); Serial.println(F(" ppm"));
  delay(500);
}
