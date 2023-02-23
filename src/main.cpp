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
bool transmitted = false;

//function declarations
void connectMQTT();
void publishValue(bool emitted);
void messageReceived(String &topic, String &payload);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password); // begin WiFi connection
    // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  pinMode(PIN, INPUT);

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

  inputValue = digitalRead(PIN);
  if(inputValue == HIGH){
    emitted = true;
    Serial.print("\nAccident Detected");
  }

  if(emitted && !transmitted){
    publishValue(emitted);
    Serial.print("\nData Transmitted");
    transmitted = true;
  }

  Serial.print("\nMeasurment ...");
  Serial.print("\n-> Value: " + String(inputValue) + " emitted: " + String(emitted) + " transmitted: " + String(transmitted));
  delay(100);
  emitted = false;
}

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

void publishValue(bool emitted){

  Serial.print("\nCurrent emitter value: ");
  Serial.print(emitted);
  mqttClient.publish("post-box-emitter/emitted", emitted ? "ON" : "OFF");
}

void messageReceived(String &topic, String &payload) {
  if(payload == "OFF"){
    transmitted = false;
    emitted = false;
    Serial.print("\nRESET!!!");
  }
}