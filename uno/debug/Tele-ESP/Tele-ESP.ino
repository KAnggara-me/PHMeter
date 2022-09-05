/*
  Name:		    echoBot.ino
  Created:	    12/21/2017
  Author:	    Stefano Ledda <shurillu@tiscalinet.it>
  Description: a simple example that check for incoming messages
              and reply the sender with the received message
*/
#include "CTBot.h"
CTBot myBot;

String ssid  = "Kos_oren"    ; // REPLACE mySSID WITH YOUR WIFI SSID
String pass  = "masihyanglama"; // REPLACE myPassword YOUR WIFI PASSWORD, IF ANY
String token = "5517337437:AAEqMcOmMcsopjpBr4y8uFhx9VdiOgbeMwg"   ; // REPLACE myToken WITH YOUR TELEGRAM BOT TOKEN
float angka;
float setpoint = 0.70;
bool notif = true;

// Milis
unsigned long previousMillis = 0;

void setup() {
  angka = 0.70;
  // initialize the Serial
  Serial.begin(115200);
  Serial.println("Starting TelegramBot...");

  // connect the ESP8266 to the desired access point
  myBot.wifiConnect(ssid, pass);

  // TODO: For Developmen, Remove on Production
  myBot.setIP("192.168.1.234", "192.168.1.1", "255.255.255.0", "8.8.8.8", "8.8.4.4");

  // set the telegram bot token
  myBot.setTelegramToken(token);

  // check if all things are ok
  if (myBot.testConnection()) {
    Serial.println("\ntestConnection OK");
  } else {
    Serial.println("\ntestConnection NOK");
  }
}

void loop() {
  Serial.println("Angka = " + String(angka));
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 5000) {
    previousMillis = currentMillis;
    ran();
    Serial.print(F("Cek Sensor kirim => "));
    Serial.println(String(angka));
    Serial.println("Notif => " + String(notif));
  }
  delay(1500);
}

void ran() {
  angka = random(65, 100) / 100.0;
  cek();
}

void cek() {
  if (angka > setpoint) {
    if (notif) {
      kirim("High");
      notif = false;
    }
    Serial.println("Tinggi");
  }

  if (angka <= setpoint) {
    if (!notif) {
      kirim("Normal");
      notif = true;
    }
    Serial.println("Normal");
  }
}

void kirim(String msg) {
  String pesan = msg + "\n\nAngka sekarang adalah = ";
  pesan +=  String(angka);
  myBot.sendMessage(1000839880, pesan);
}
