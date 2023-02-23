# MQTT - Postbox Emitter

This project is created to emit if there are new mails in your physical postbox by using MQTT. To achive this, the following hardware is required:

- ESP8266 (NodeMCU)
- SW-420 (vibration detector)

As you might have noticed already, this is a [PlatformIO](https://platformio.org) project. Simply clone the repo, open it in PlatformIO, do some configuration (as explained below) and upload the code onto your NodeMCU.



### Configuration
To files need to be adjusted. The general config file found in `src/config.h` 

````
#define PIN D2 // <- IO PIN
#define MQTT_BROKER_ADDR "192.168.0.42" <- MQTT Broker address
````
 And the secrets placed in `src/secrets.h` (file need to be created first) 


### Secrets
````
#define SECRET_WIFI_SSID "Secret-WiFi-SSID"
#define SECRET_WIFI_PASS "Secret-WiFi-PW"
#define MQTT_USER_NAME "MQTT-USER"
#define MQTT_USER_PW "MQTT-PW"
````


## HomeAssistant

The following code is an example of an HomeAsstiant integration by using the MQTT Client.

````
mqtt:
  button:
    - command_topic: "post-box-emitter/emitted"
      payload_press: "OFF"
      unique_id: "pbe_reset_001"
      device:
        name: "PostBoxEmitter"
        model: "SimpleBetaModel"
        manufacturer: "PGE-Projects"
        identifiers: "pbe_001"
      name: "PGE PostBox Emitter Reset"
      object_id: "pge_pbe_reset"
      icon: "mdi:restart"
  binary_sensor:
    - state_topic: "post-box-emitter/emitted"
      availability_topic: "post-box-emitter/status"
      unique_id: "pbe_md_001"
      device:
        name: "PostBoxEmitter"
        model: "SimpleBetaModel"
        manufacturer: "PGE-Projects"
        identifiers: "pbe_001"
      device_class: "occupancy"
      icon: "mdi:mailbox"
      name: "PGE PostBox Emitter Mail Detection"
      object_id: "pge_pbe_md"
````
