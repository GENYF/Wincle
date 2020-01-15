#include "ThingSpeak.h"
#include <ESP8266WiFi.h>
#include <Servo.h>

char ssid[] = "GDIP";          //  your network SSID (name) 
char pass[] = "********";   // your network password

int servoPin = D1; 
int angle = 0;

int readNum = 0;
int chance = 0;
int last = 2;
Servo servo;

int status = WL_IDLE_STATUS;
WiFiClient  client;
int readValue; // variable to save channel field reading
int UserValue;
    
unsigned long myChannelNumber = 358487; // modify this with your own Channel Number
const char * myReadAPIKey = "4V4ZMSBWYDZHHY7Q";

void setup() {
  Serial.begin(9600); // for debugging reasons
  WiFi.begin(ssid, pass);
  ThingSpeak.begin(client);
  servo.attach(servoPin);
}

void loop() {
  UserValue = ThingSpeak.(myChannelNumber, 6, myReadAPIKey);
  if(UserValue > 0){
    break;
  }
  else{
    chance++;
    if(chance > 2){
      if (readNum == 0 && angle == 0){
        for(angle = 0; angle < 60; angle++) 
        { 
          servo.write(angle); 
          delay(10); 
        } 
      }
      else if(readNum == 2 && angle == 60){
        for(angle = 60; angle > 0; angle--) 
        { 
         servo.write(angle); 
         delay(10); 
        } 
      }
      chance = 0;
      readNum = 0;
    }
    else
    {
      readValue = ThingSpeak.readIntField(myChannelNumber, 5, myReadAPIKey);
      if(last != readValue){
        chance = 0;
        readNum = 0;
        last = readValue;
      }
      else{
        readNum += readValue;
        last = readValue;
      }
    }
    Serial.print("readValue = "); // debugging instrument
    Serial.println(readValue);    // debugging instrument
    Serial.print("readNum = "); // debugging instrument
    Serial.println(readNum);    // debugging instrument
  }
}
