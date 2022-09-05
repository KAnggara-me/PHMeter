#include <CTBot.h>
#include <MQUnifiedsensor.h>
#include <LiquidCrystal_I2C.h>
#include "config.h"
#include "time.h"

// Connection Setting
const char* ssid = WIFI_SSID;
const char* pass = WIFI_PASS;

// Time Section
char timeWeekDay[20];

// Variable
float phAir, Amonia;
bool PHTinggi = false;
bool PHRendah = false;
bool AmoniaTinggi = false;

// Milis
unsigned long previousMillis = 0;

// Telegram Object
CTBot myBot;

// LCD Object
LiquidCrystal_I2C lcd(0x27, 16, 2);

// MQ-135 Object
MQUnifiedsensor MQ("ESP-32", 3.3, 12, MQPin, "MQ-135");

void setup() {
  Serial.begin(115200); // initialize serial for debugging
  lcdInit(); // OK
  botSetup(); // OK
  timeSetup(); // OK
  MQInit();
  bacaPH();
  bacaAmonia();
  LCDPrint(phAir, Amonia);
}

void loop() {
  unsigned long currentMillis = millis();
  //  Check Sensors every 30 seconds
  if (currentMillis - previousMillis >= 30000) {
    Serial.println(F("Cek Sensor"));
    previousMillis = currentMillis;
    bacaPH();
    bacaAmonia();
    cekPH();
    cekAmonia();

  }
  checkMsg();
  delay(500); // wait 500 milliseconds
}

/*==============================================================================
  ============================== GPIO INIT SECTION =============================
  ==============================================================================*/
// delay for prepare MQ sensor
void lcdInit() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Starting"));
  delay(2000);
  gpioInit();
  for (int i = 0; i <= 8; i++) {
    lcd.print(F("."));
    delay(2000);
  }
  lcd.setCursor(0, 1);
  for (int i = 0; i <= 8; i++) {
    lcd.print(F("."));
    delay(2000);
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Loading..."));
  delay(1000);
}

// I/O Config
void gpioInit() {
  pinMode(MQPin, INPUT);
  pinMode(PHMeter, INPUT);
  pinMode(phPlus, OUTPUT);
  pinMode(phMinus, OUTPUT);
  digitalWrite(phPlus, HIGH);
  digitalWrite(phMinus, HIGH);
}

// Setup Telegrem bot and board connection
void botSetup() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print(F("Connecting...."));
  lcd.setCursor(0, 1);
  lcd.print(F("To: "));
  lcd.print(ssid);

  // Connecting to a WiFi network
  Serial.print(F("Connecting to "));
  Serial.println(ssid);
  myBot.wifiConnect(ssid, pass);

  // Connect to Telegram Server
  myBot.setTelegramToken(token);

  // TODO: For Developmen, Remove on Production
  myBot.setIP("192.168.1.234", "192.168.1.1", "255.255.255.0", "8.8.8.8", "8.8.4.4");

  // check if all things are ok
  if (myBot.testConnection()) {
    myBot.sendMessage(GroupID, "Connected to => " + String(ssid));
    Serial.println(F("\nConnection OK"));
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print(F("Connection OK!"));
  } else {
    Serial.println(F("\nConnection Not OK"));
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Conection Not OK"));
    lcd.setCursor(0, 1);
    lcd.print(F("Check Internet !"));
  }
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
  return R0 / (sample * 10);
}

// MQ-135 Initialization
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

/*==============================================================================
  ================================ TIME SECTION ===============================
  ==============================================================================*/
void timeSetup() {
  configTime(25200, 0, "0.id.pool.ntp.org");
  printLocalTime();
}

void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println(F("Failed to obtain time"));
    return;
  }
  strftime(timeWeekDay, 20, "%X %x", &timeinfo);
  Serial.println(timeWeekDay);
}

/*==============================================================================
  ============================== TELEGRAM SECTION ==============================
  ==============================================================================*/
// Check if any incoming msg
void checkMsg() {
  // check if all things are ok
  if (!myBot.testConnection()) {
    Serial.println(F("\nConnection Not OK"));
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Conection Not OK"));
    lcd.setCursor(0, 1);
    lcd.print(F("Check Internet !"));
  }
  // a variable to store telegram message data
  TBMessage msg;

  // if there is an incoming message...
  if (CTBotMessageText == myBot.getNewMessage(msg)) {
    int id;
    // check if the message is a text message
    if (msg.messageType == CTBotMessageText) {
      printLocalTime();
      Serial.println(F("Incoming Message: "));
      Serial.println(msg.text);
      // check if the message comes from a chat group (the group.id is negative)
      if (msg.group.id < 0) {
        id = msg.group.id;
      } else {
        id = msg.sender.id;
      }
      proccessMsg(id, msg.text);
    }
  } else {
    Serial.println(F("."));
  }
}

// Proccess incoming msg
void proccessMsg(int id, String msg) {
  // if incoming msg contain "/menu" or "/start"
  if ((msg.indexOf("/menu") == 0) || (msg.indexOf("/start") == 0)) {
    String menu = "Welcome to PHMeter Bot\n\n";
    menu += "/status : Show sensor status\n";
    menu += "/menu : Show this menu\n\n";
    menu += "Dikirim Pada: " + String(timeWeekDay);
    myBot.sendMessage(id, menu);
  }

  // if incoming msg contain "/status"
  if (msg.indexOf("/status") == 0) {
    String pesan = "Status Sensor Saat ini\n\n";
    pesan += "PH Air => " + String(phAir);
    pesan += "\nAmonia => " + String(Amonia);
    pesan += "\n\nDikirim Pada: " + String(timeWeekDay);
    myBot.sendMessage(id, pesan);
  }
}

/*==============================================================================
  ============================= READ SENSOR SECTION ============================
  ==============================================================================*/
// Read Water PH
void bacaPH() {
  int measurings = 0;
  for (int i = 0; i < 12; i++) {
    measurings += analogRead(PHMeter);
    delay(10);
  }
  float voltage = (float(measurings / 12) * (3.3 / 4095.0), 1) ;
  phAir = ph(voltage);
  // phAir = random(600, 800) / 100.0;
  Serial.println("PH Saat ini => " + String(phAir) + " | RAW " + String(voltage));
}

// Check Water PH
void cekPH() {
  if (phAir > PHMax) {
    lcdPH("TINGGI", phAir);
    Serial.println(F("PH Air Tinggi"));
    kurangiPH();
    if (!PHTinggi) {
      myBot.sendMessage(GroupID, "PH Air Tinggi => " + String(phAir));
      PHTinggi = true;
      PHRendah = false;
    }
  } else if (phAir < PHMin) {
    lcdPH("RENDAH", phAir);
    Serial.println(F("PH Air Rendah"));
    tambahPH();
    if (!PHRendah) {
      myBot.sendMessage(GroupID, "PH Air Rendah => " + String(phAir));
      PHRendah = true;
      PHTinggi = false;
    }
  } else if ((phAir > PHMin) && (phAir < PHMax) && (PHTinggi || PHRendah)) {
    LCDPrint(phAir, Amonia);
    Serial.println(F("PH Air Telah Normal"));
    myBot.sendMessage(GroupID, "PH Air Telah Normal => " + String(phAir));
    PHTinggi = false;
    PHRendah = false;
  }
}

// Convert sensor value to PH
float ph (float voltage) {
  return 7 + (2.5 - voltage);
}

// Read MQ-135
void bacaAmonia() {
  MQ.update();
  MQ.setA(102.2 ); MQ.setB(-2.473); //NH4
  float NH4 = MQ.readSensor();
  Amonia = NH4;
  Serial.print(F("NH4 => ")); Serial.print(NH4); Serial.println(F(" ppm"));
}

// Check MQ-135 Value
void cekAmonia() {
  if (Amonia > amoniaMax) {
    LCDAmonia(Amonia);
    Serial.println(F("Kadar Amonia Tinggi"));
    if (!AmoniaTinggi) {
      myBot.sendMessage(GroupID, "Kadar Amonia Tinggi => " + String(Amonia));
      AmoniaTinggi = true;
    }
  } else if ((Amonia < amoniaMax) && (AmoniaTinggi)) {
    LCDPrint(phAir, Amonia);
    Serial.println(F("PH Air Telah Normal"));
    myBot.sendMessage(GroupID, "PH Air Telah Normal => " + String(phAir));
    AmoniaTinggi = false;
  }
}

/*==============================================================================
  ============================== CONTROL SECTION ===============================
  ==============================================================================*/
void tambahPH() {
  Serial.println(F("Menaikan PH Air"));
  digitalWrite(phPlus, LOW);
  delay(5000);
  digitalWrite(phPlus, HIGH);
}

void kurangiPH() {
  Serial.println(F("Menurunkan PH Air"));
  digitalWrite(phMinus, LOW);
  delay(5000);
  digitalWrite(phMinus, HIGH);
}

/*==============================================================================
  =========================== PRINT TO LCD SECTION =====+=======================
  ==============================================================================*/
void LCDAmonia(float amonia) {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("AMONIA TINGGI!");
  lcd.setCursor(0, 1);
  lcd.print(F("Amonia: "));
  lcd.print(amonia);
  lcd.print(F(" ppm"));
}

void lcdPH(String msg, float PH) {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("PH AIR " + msg + "!");
  lcd.setCursor(1, 1);
  lcd.print(F("PH : "));
  lcd.print(PH);
}

void LCDPrint(float ph, float amonia) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("PH    : "));
  lcd.print(ph);
  lcd.setCursor(0, 1);
  lcd.print(F("Amonia: "));
  lcd.print(amonia);
  lcd.print(F(" ppm"));
}
