#include <CTBot.h>

#include "config.h"
#include "time.h"

CTBot myBot;

const char* ssid = WIFI_SSID;
const char* pass = WIFI_PASS;

// Time Section
const char* ntpServer = "0.id.pool.ntp.org";
const long  gmtOffset_sec = 25200; // GMT + 7
char timeWeekDay[20];

float phAir;
bool PHTinggi = false;
bool PHRendah = false;

// Milis
unsigned long previousMillis = 0;

void setup() {
  phAir = 7.0;
  // initialize the Serial
  Serial.begin(115200);

  // We start by connecting to a WiFi network
  Serial.print(F("Connecting to "));
  Serial.println(ssid);

  botSetup();
  timeSetup();
}

void botSetup() {
  // connect the ESP8266 to the desired access point
  myBot.wifiConnect(ssid, pass);
  myBot.setTelegramToken(token);
  //TODO: For Developmen, Remove on Production
  myBot.setIP("192.168.1.234", "192.168.1.1", "255.255.255.0", "8.8.8.8", "8.8.4.4");
  // check if all things are ok
  if (myBot.testConnection()) {
    myBot.sendMessage(GroupID, "Connected to => " + String(ssid));
    Serial.println(F("\nConnection OK"));
  } else {
    Serial.println(F("\nConnection Not OK"));
  }
  //  myBot.setParseMode(CTBotParseModeMarkdown);

}

void timeSetup() {
  // Init and get the time
  configTime(gmtOffset_sec, 0, ntpServer);
  printLocalTime();
}

void loop() {
  // a variable to store telegram message data
  TBMessage msg;

  // if there is an incoming message...
  if (CTBotMessageText == myBot.getNewMessage(msg)) {
    int id;
    // check if the message is a text message
    if (msg.messageType == CTBotMessageText) {
      printLocalTime();
      Serial.print(F("Pesan Masuk: "));
      Serial.println(msg.text);
      // check if the message comes from a chat group (the group.id is negative)
      if (msg.group.id < 0) {
        // if admin not a group
        if ((msg.text.indexOf("/calibrate") == 0)) {
          id = msg.sender.id;
        } else {
          id = msg.group.id;
        }
        showMenu(id, msg.text);
      } else {
        id = msg.sender.id;
        showMenu(id, msg.text);
      }
    }
  }
  //  cek PH Air setiap 30 detik
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 10000) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    cekPH();
  }
  // wait 500 milliseconds
  delay(500);
}


void showMenu(int id, String msg) {
  if ((msg.indexOf("/menu") == 0) || (msg.indexOf("/start") == 0)) {
    String menu = "Welcome to PHMeter Bot\n\n";
    menu += "/status : Show sensor status\n";
    menu += "/menu : Show this menu\n\n";
    menu += "Dikirim Pada: " + String(timeWeekDay);
    myBot.sendMessage(id, menu);
  }
}

void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }

  strftime(timeWeekDay, 20, "%X %x", &timeinfo);
  Serial.println(timeWeekDay);
}

void cekPH() {
  phAir = random(645, 755) / 100.0;
  Serial.println("PH Saat ini => " + String(phAir));
  if ((phAir >= PHMax) && !PHTinggi) {
    Serial.println("PH AIR Tinggi");
    myBot.sendMessage(GroupID, "PH AIR Tinggi => " + String(phAir));
    PHTinggi = true;
    PHRendah = false;
  } else if ((phAir <= PHMin) && !PHRendah) {
    Serial.println("PH AIR Rendah");
    myBot.sendMessage(GroupID, "PH AIR Rendah => " + String(phAir));
    PHRendah = true;
    PHTinggi = false;
  } else if ((phAir >= PHMin) && (phAir <= PHMax) && (PHTinggi || PHRendah)) {
    Serial.println("PH AIR Telah Normal");
    myBot.sendMessage(GroupID, "PH AIR Telah Normal => " + String(phAir));
    PHTinggi = false;
    PHRendah = false;
  }
}
