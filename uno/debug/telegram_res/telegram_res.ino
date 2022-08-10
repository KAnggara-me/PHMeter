#include <CTBot.h>
#include "config.h"

CTBot myBot;

const char* ssid = WIFI_SSID;
const char* pass = WIFI_PASS;


void setup() {
  // initialize the Serial
  Serial.begin(115200);

  // We start by connecting to a WiFi network
  Serial.print(F("Connecting to "));
  Serial.println(ssid);

  // connect the ESP8266 to the desired access point
  myBot.wifiConnect(ssid, pass);
  myBot.setTelegramToken(token);

  // check if all things are ok
  if (myBot.testConnection()) {
    myBot.sendMessage(GroupID, "Connected to " + String(ssid));
    Serial.println(F("\nConnection OK"));
  } else {
    Serial.println(F("\nConnection Not OK"));
  }
}

void loop() {
  // a variable to store telegram message data
  TBMessage msg;

  // if there is an incoming message...
  if (CTBotMessageText == myBot.getNewMessage(msg)) {
    int id;
    // check if the message is a text message
    if (msg.messageType == CTBotMessageText) {
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
  // wait 500 milliseconds
  delay(500);
}


void showMenu(int id, String msg) {
  if ((msg.indexOf("/menu") == 0) || (msg.indexOf("/start") == 0)) {
    String menu = "Welcome to PHMeter Bot\n\n/status : Show status\n/menu : Show this menu\n/ph_plus : Add Water PH\n/ph_minus : Reduce Water PH\n";
    myBot.sendMessage(id, menu);
  }
}
