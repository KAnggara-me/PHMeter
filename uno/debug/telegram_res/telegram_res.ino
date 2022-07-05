#include <CTBot.h>
#include "auth.h"

CTBot myBot;

const char* ssid = WIFI_SSID;
const char* pass = WIFI_PASS;

void setup() {
  // initialize the Serial
  Serial.begin(115200);
  Serial.println("Starting TelegramBot...");

  // connect the ESP8266 to the desired access point
  myBot.wifiConnect(ssid, pass);

  // set the telegram bot token
  myBot.setTelegramToken(token);

  // check if all things are ok
  if (myBot.testConnection()) {
    myBot.sendMessage(AdminId, "Connected to " + String(ssid));
    Serial.println("\nConnection OK");
  } else {
    Serial.println("\nConnection Not OK");
  }
}

void loop() {
  // a variable to store telegram message data
  TBMessage msg;

  // if there is an incoming message...
  if (CTBotMessageText == myBot.getNewMessage(msg)) {
    int id;
    if ((msg.text.indexOf("/menu") == 0) || (msg.text.indexOf("/start") == 0)) {
      id = msg.sender.id;
      showMenu(id);
    }
  }

  // wait 500 milliseconds
  delay(500);
}


void showMenu(int id) {
  String menu = "Welcome to PHMeter Bot\n\n/status : Show status\n/menu : Show this menu\n/ph_plus : Add Water PH\n/ph_minus : Reduce Water PH\n";
  myBot.sendMessage(id, menu);
}
