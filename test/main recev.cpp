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
const char* mqttTopic = "kelasiotesp/astuti";
const char* mqttTopic2 = "kelasiotesp/astuti/button";
const char* clientId = "KAZARACH";

WiFiClient espClient;
PubSubClient client(espClient);
Servo myservo;

typedef struct struct_distance {
    float distance_cm;
    bool servo_moved;
} struct_distance;

struct_distance sensorData;

void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  if (len == sizeof(struct_distance)) {
    memcpy(&sensorData, incomingData, sizeof(struct_distance));
    
    Serial.print("Bytes received: ");
    Serial.println(len);
    Serial.print("Distance in cm: ");
    Serial.println(sensorData.distance_cm);

    if (sensorData.distance_cm > 10) {
      myservo.write(90);
      delay(1000);
      myservo.write(0);
      sensorData.servo_moved = true;

      if (client.connected()) {
        char message[50];
        snprintf(message, sizeof(message), "%.2f %d", sensorData.distance_cm, sensorData.servo_moved ? 1 : 0);
        client.publish(mqttTopic, message);
    }
    } else {
      sensorData.servo_moved = false;
      if (client.connected()) {
        char message[50];
        snprintf(message, sizeof(message), "%.2f %d", sensorData.distance_cm, sensorData.servo_moved ? 1 : 0);
        client.publish(mqttTopic, message);
    } 
    } 
      Serial.print("Servo state: ");
      Serial.println(sensorData.servo_moved);
  } else {
    Serial.println("Received data size mismatch!");
  }
}
void callback2(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  // Convert payload to a string
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  Serial.println(message);

  if (String(topic) == mqttTopic2) {
    if (message.equals("1")) {
      myservo.write(90);
      delay(1000);
      myservo.write(0); 
      Serial.print("Interrupt Signal: ");
      Serial.println(message);
    }
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
    Serial.print(client.state());
    Serial.print("Attempting MQTT connection...");
    if (client.connect(clientId)) {
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
  client.subscribe(mqttTopic2);
  client.setCallback(callback2);

  myservo.attach(2); 
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}