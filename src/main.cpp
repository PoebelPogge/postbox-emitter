#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <MQTT.h>
#include "secrets.h"
#include "config.h"

const char *ssid = SECRET_WIFI_SSID;
const char *password = SECRET_WIFI_PASS;

WiFiClient net;
MQTTClient mqttClient;

int inputValue = 0;

bool emitted = false;

//function declarations
void connectWiFi();
void connectMQTT();
void publishValue(bool emitted);
void messageReceived(String &topic, String &payload);

void setup() {
  Serial.begin(115200);
  pinMode(PIN, INPUT);

  connectWiFi();

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Connecting to MQTT broker...");
  mqttClient.begin(MQTT_BROKER_ADDR, net);
  mqttClient.onMessage(messageReceived);
  mqttClient.setWill("post-box-emitter/status", "offline");
  connectMQTT();
  Serial.println("Connection to MQTT brocker established");

  Serial.println("Device started!");
}

void loop() {
  mqttClient.loop();

  if(!mqttClient.connected()){
    Serial.println("Lost connection to MQTT broker, reconnecting...");
    connectMQTT();
  }
  Serial.print("\nMeasurment ...");
  emitted = digitalRead(PIN);

  if(emitted){
    Serial.print("\nAccident Detected");
    publishValue(emitted);
    Serial.print("\nData Transmitted");
    emitted = false;
  }
  delay(500);
}


//Establish MQTT Connection
void connectMQTT(){
  while (!mqttClient.connect("post-box-emitter", MQTT_USER_NAME, MQTT_USER_PW))
  {
    Serial.print(".");
    delay(1000);
  }
  mqttClient.publish("post-box-emitter/status", "online");
  mqttClient.publish("post-box-emitter/emitted", emitted ? "ON" : "OFF");
  mqttClient.publish("post-box-emitter/localIp", WiFi.localIP().toString());

  mqttClient.subscribe("post-box-emitter/emitted");
}

void connectWiFi(){
  WiFi.begin(ssid, password); // begin WiFi connection
    // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
}

// publish mqtt value to broker
void publishValue(bool emitted){

  Serial.print("\nCurrent emitter value: ");
  Serial.print(emitted);
  mqttClient.publish("post-box-emitter/emitted", emitted ? "ON" : "OFF");
}

// receive message from broker
void messageReceived(String &topic, String &payload) {
  if(payload == "OFF"){
    emitted = false;
    Serial.print("\nRESET!!!");
  }
}