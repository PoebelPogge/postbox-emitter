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
void disconnectWiFi();
void connectMQTT();
void publishValue(bool emitted);
void messageReceived(String &topic, String &payload);

//initial Setup
void setup() {
  Serial.begin(115200);
  pinMode(PIN, INPUT);

  Serial.println("Device started!");
}


// programm loop
void loop() {
  Serial.println("Measurment ...");
  emitted = digitalRead(PIN);

  if(emitted){
    Serial.println("Accident Detected");
    connectWiFi();
    connectMQTT();
    mqttClient.loop();
    publishValue(emitted);
    Serial.println("Data Transmitted");
    disconnectWiFi();
    emitted = false;
  }
  delay(500);
}

// establishing Wifi Connection
void connectWiFi(){
  Serial.print("\nConnecting to WiFi");
  WiFi.forceSleepWake();
  delay(1);
   WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password); // begin WiFi connection
    // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// disconnecting WiFi Connection
void disconnectWiFi(){
  WiFi.mode( WIFI_OFF );
  WiFi.forceSleepBegin();
  Serial.println("WiFi is down");
}

//Establish MQTT Connection
void connectMQTT(){
  Serial.println("Connecting to MQTT broker...");

  mqttClient.begin(MQTT_BROKER_ADDR, net);
  // mqttClient.onMessage(messageReceived);
  mqttClient.setWill("post-box-emitter/status", "offline");

  while (!mqttClient.connect("post-box-emitter", MQTT_USER_NAME, MQTT_USER_PW))
  {
    Serial.print(".");
    delay(1000);
  }
  mqttClient.publish("post-box-emitter/status", "online");
  mqttClient.publish("post-box-emitter/emitted", emitted ? "ON" : "OFF");
  mqttClient.publish("post-box-emitter/localIp", WiFi.localIP().toString());

  mqttClient.subscribe("post-box-emitter/emitted");

  Serial.println("Connection to MQTT brocker established");
}

// publish mqtt value to broker
void publishValue(bool emitted){
  mqttClient.publish("post-box-emitter/emitted", emitted ? "ON" : "OFF");
}

// receive message from broker
void messageReceived(String &topic, String &payload) {
  if(payload == "OFF"){
    emitted = false;
    Serial.print("\nRESET!!!");
  }
}