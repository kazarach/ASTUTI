#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Replace this with the MAC address of the sender ESP8266
uint8_t receiverMACAddress[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

// WiFi and MQTT configurations
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";
const char* mqtt_server = "your_MQTT_broker_IP";
const char* mqtt_username = "your_MQTT_username";
const char* mqtt_password = "your_MQTT_password";
const char* mqtt_client_id = "esp8266_receiver";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // MQTT setup
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register peer (sender ESP8266)
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, senderMACAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  // Register callback function to receive data
  esp_now_register_recv_cb(onDataReceived);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  // Do other tasks if needed
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Handle incoming MQTT messages if required
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(mqtt_client_id, mqtt_username, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// Callback function to handle received data
void onDataReceived(const uint8_t *senderMac, const uint8_t *data, int dataLength) {
  // Check if the received data is from the expected sender
  if (memcmp(senderMac, senderMACAddress, 6) == 0) {
    // Convert received data to a string
    String receivedData = "";
    for (int i = 0; i < dataLength; i++) {
      receivedData += (char)data[i];
    }

    // Send the received data to MQTT
    if (client.connected()) {
      client.publish("esp8266/data", receivedData.c_str());
    } else {
      Serial.println("MQTT not connected!");
    }
  } else {
    Serial.println("Received data from an unknown sender");
  }
}
