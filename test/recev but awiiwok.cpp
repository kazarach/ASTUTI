#include <ESP8266WiFi.h>
#include <espnow.h>
#include <PubSubClient.h>
#include <Servo.h>

// Update with your WiFi and MQTT broker details
const char* ssid = "Tel-U 49";
const char* password = "capstone49";
const char* mqttServer = "broker.emqx.io";
const int mqttPort = 1883;
const char* mqttUsername = "emqx";
const char* mqttPassword = "public";
const char* mqttTopic = "kelasiotesp/hasbi/data/astuti";
const char* mqttTopic2 = "kelasiotesp/hasbi/data/astuti/button";

WiFiClient espClient;
PubSubClient client(espClient);
Servo myservo;

// Structure example to receive distance data
typedef struct struct_distance {
    float distance_cm;
    bool servo_moved; // New field to indicate servo movement
} struct_distance;

// Create a struct_distance called sensorData
struct_distance sensorData;

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  if (len == sizeof(struct_distance)) {
    memcpy(&sensorData, incomingData, sizeof(struct_distance));
    
    Serial.print("Bytes received: ");
    Serial.println(len);
    Serial.print("Distance in cm: ");
    Serial.println(sensorData.distance_cm);
    Serial.print("Servo state: ");
    Serial.println(sensorData.servo_moved);

    // Move the servo if distance is over 10cm
    if (sensorData.distance_cm > 10) {
      myservo.write(90); // Change the angle as needed
      delay(1000); // Adjust the delay as needed for servo movement
      myservo.write(0); // Reset servo position
      sensorData.servo_moved = true; // Update the servo state
    } else {
      sensorData.servo_moved = false; // Reset the servo state if not moved
    }

    Serial.print("Servo state: ");
    Serial.println(sensorData.servo_moved);

    // Send only the distance in centimeters to the MQTT broker
    
  } else {
    Serial.println("Received data size mismatch!");
  }
}

void setupWiFi() {
  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
}

void setup() {
  Serial.begin(115200);
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  setupWiFi();

  client.setServer(mqttServer, mqttPort);

 
  esp_now_init();

  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
  client.subscribe(mqttTopic2);
  client.setCallback(callback2);

  myservo.attach(D1); // Attach servo signal pin to GPIO pin D1
}

void loop() {
  client.loop();
}