#include <ESP8266WiFi.h>
#include <espnow.h>
#include <PubSubClient.h>

// Update with your WiFi and MQTT broker details
const char* ssid = "Tel-U 49";
const char* password = "capstone49";
const char* mqttServer = "broker.emqx.io";
const int mqttPort = 1883;
const char* mqttUsername = "emqx";
const char* mqttPassword = "public";
const char* mqttTopic = "kelasiotesp/hasbi/data/astuti";

WiFiClient espClient;
PubSubClient client(espClient);

// Structure example to receive distance data
typedef struct struct_distance {
    float distance_cm;
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


    // Send only the distance in centimeters to the MQTT broker
    if (client.connected()) {
      char message[10]; // Adjust the size as needed
      snprintf(message, sizeof(message), "%.2f", sensorData.distance_cm);
      client.publish("kelasiotesp/hasbi/data/astuti", message);
    }
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

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client", mqttUsername, mqttPassword)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  setupWiFi();

  client.setServer(mqttServer, mqttPort);

  if (!client.connected()) {
    reconnect();
  }

  esp_now_init();

  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
