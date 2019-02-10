#include <WiFi.h>
#include <ESPmDNS.h>
#include <PubSubClient.h>
#include <SPI.h>
#include "LedMatrix.h"
#define NUMBER_OF_DEVICES 4 //number of led matrix connect in series
#define CS_PIN 5
#define CLK_PIN 18
#define MISO_PIN 2 //we do not use this pin just fill to match constructor
#define MOSI_PIN 23

const char* SSID = "ssid"; // WiFi SSID
const char* PASSWORD = "pass"; // WiFi password
const char* MQTT_SERVER = "mqtt.server.here";
const char* MQTT_USER = "mqttuser";
const char* MQTT_PASS = "mqttpass";
const char* MQTT_TOPIC = "the/topic/here";
const char* INITTEXT = "Occupied"; // text to be displayed endlessly, MQTT/WiFi not used in this demo
const int SCROLLSPEED = 10;
const int BRIGHTNESS = 3; // Brightness 0 ... 15

LedMatrix ledMatrix=LedMatrix(NUMBER_OF_DEVICES, CLK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);

WiFiClient espClient;
PubSubClient client(espClient);
//long lastMsg = 0;
char msg[50];
//int value = 0;

void setup() {
  Serial.begin(9600);
  //setup_wifi();
  //client.setServer(MQTT_SERVER, 1883);
  //client.setCallback(callback);
  ledMatrix.init();
  ledMatrix.setIntensity(BRIGHTNESS);
  ledMatrix.setText(INITTEXT);
  //default font has 7 pixel width. calculate # of characters x 7 for i max value
  //add NUMBER_OF_DEVICES*pixel per device for outscrolling:
  int length = strlen(INITTEXT);
  for (int i = 0; i < length * 7 + 8 * NUMBER_OF_DEVICES; i++) {
    ledMatrix.clear();
    ledMatrix.scrollTextLeft();
    ledMatrix.drawText();
    ledMatrix.commit();
    delay(SCROLLSPEED);
  }
}
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to SSID ");
  Serial.println(SSID);
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String msg;
  for (int i = 0; i < length; i++) {
     Serial.print((char)payload[i]);
     msg += (char)payload[i];
  }
  Serial.println(" ");
  //uncomment ledMatrix.clear(); if new message should 
  //clear the display rather than "shifting over" the old:
  ledMatrix.clear(); 
  ledMatrix.setText(msg);
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    uint64_t chipid = ESP.getEfuseMac(); // MAC address of ESP32
    uint16_t chip = (uint16_t)(chipid>>32);
    char clientid[25];
    snprintf(clientid,25,"Matrix-Display-%04X",chip);
    if (client.connect(clientid,MQTT_USER,MQTT_PASS)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("Say", "-t 'hello world'");
      // ... and resubscribe
      client.subscribe(MQTT_TOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void scrollOnce(char* msg) {
int length = strlen(msg);
  for (int i = 0; i < length * 7 + 8 * NUMBER_OF_DEVICES; i++) {
    ledMatrix.scrollTextLeft();
    ledMatrix.drawText();
    ledMatrix.commit();
    delay(SCROLLSPEED);
  }
}
void loop() {
  /*if (!client.connected()) {
  reconnect();
  }
  client.loop(); */
  scrollOnce(msg);
  //delay(2000);
}


