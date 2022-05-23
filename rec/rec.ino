/*
  ESP-NOW Remote Sensor - Receiver (Multiple Version)
  esp-now-rcv.ino
  Receives Temperature & Humidity data from other ESP32 via ESP-NOW
  
  DroneBot Workshop 2022
  https://dronebotworkshop.com
*/

// Include required libraries
#include <WiFi.h>
#include <esp_now.h>
#include "ThingSpeak.h"
#include <PololuOLED.h>

const char* ssid = "SHREY-ACER 1331";   // your network SSID (name) 
const char* password = "shrey020701";

WiFiClient  client;

unsigned long myChannelNumber = 1727766;
const char * myWriteAPIKey = "VCC7ORGZL1IT8T6M";

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 2000;

// Define data structure
typedef struct struct_message {
  //float x;
  //float y;
  //float z;
  //int b;
  //int c;
  float d;
  int e;
} struct_message;

// Create structured data object
struct_message myData;

PololuSH1106 display(18, 23, 25, 19, 5);

// Callback function
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) 
{
  // Get incoming data
  memcpy(&myData, incomingData, sizeof(myData));
  
  // Print to Serial Monitor
  Serial.print("Data received: ");
  Serial.println(len);
  Serial.print("Node: ");
  Serial.println(myData.e);
  /*Serial.print("AccX: ");
  Serial.println(myData.x);
  Serial.print("AccY: ");
  Serial.println(myData.y);
  Serial.print("AccZ: ");
  Serial.println(myData.z);
  Serial.print("Moisture: ");
  Serial.println(myData.b);
  Serial.print("Vibration: ");
  Serial.println(myData.c);*/
  Serial.print("Landslide Index: ");
  Serial.println(myData.d);
  Serial.println();
}
 
void setup() {
  // Set up Serial Monitor
  Serial.begin(115200);
  display.setLayout21x8();
  delay(1000);
  
  // Start ESP32 in Station mode
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);

  // Initalize ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
   
  // Register callback function
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    
    // Connect or reconnect to WiFi
    if(WiFi.status() != WL_CONNECTED){
      Serial.print("Attempting to connect");
      while(WiFi.status() != WL_CONNECTED){
        WiFi.begin(ssid, password); 
        delay(5000);     
      } 
      Serial.println("\nConnected.");
    }
    if (myData.e == 1)
    {
      ThingSpeak.setField(1, myData.d);
    }
    else if (myData.e == 2)
    {
      ThingSpeak.setField(2, myData.d);
    }
    /*ThingSpeak.setField(1, myData.e);
    //ThingSpeak.setField(1, temperatureF);
    ThingSpeak.setField(2, myData.y);
    ThingSpeak.setField(3, myData.z);
    ThingSpeak.setField(4, myData.d);
    ThingSpeak.setField(5, myData.c);
    ThingSpeak.setField(6, myData.b);
    ThingSpeak.setField(7, myData.e);*/

    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    if(x == 200){
      Serial.println("Channel update successful.");
    }
    else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    lastTime = millis();
  }
  
  display.gotoXY(0, 0);
  display.print("Node: ");
  //display.gotoXY(0, 7);
  display.print(myData.e);
  
  display.gotoXY(0, 1);
  display.print("Landslide Index: ");
  display.gotoXY(0, 2);
  display.print(myData.d);
  
  /*display.gotoXY(0, 2);
  display.print("AccY: ");
  //display.gotoXY(1, 7);
  display.print(myData.y);
  
  display.gotoXY(0, 3);
  display.print("AccZ: ");
  //display.gotoXY(2, 7);
  display.print(myData.z);
  
  display.gotoXY(0, 4);
  display.print("Moisture: ");
  //display.gotoXY(3, 11);
  display.print(myData.b);
  
  display.gotoXY(0, 5);
  display.print("Vibration: ");
  //display.gotoXY(4, 12);
  display.print(myData.c);
  
  display.gotoXY(0, 6);
  display.print("Rain: ");
  //display.gotoXY(5, 7);
  display.print(myData.d);*/
  }
