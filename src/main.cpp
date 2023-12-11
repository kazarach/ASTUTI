// #include <Arduino.h>
// #include "HX711.h"

// const int DOUT_PIN = D7;  // Replace with your ESP8266 GPIO pin connected to DT
// const int SCK_PIN = D8;   // Replace with your ESP8266 GPIO pin connected to SCK

// HX711 scale;

// void setup() {
//     Serial.begin(9600);
//     scale.begin(DOUT_PIN, SCK_PIN); // Initialize HX711 object with pins
//     scale.set_scale(1.0);           // Set calibration factor if needed
//     scale.tare();                   // Reset the scale to 0

//     delay(100); // Adding a small delay after initialization
// }

// void loop() {
//     float weight = scale.get_units(10); // Read weight (change 10 to the average times)
//     Serial.print("Weight: ");
//     Serial.println(weight);
//     delay(1000); // Adjust the delay based on your needs
// }

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "HX711.h"

// WiFi
const char *ssid = "KAZ"; // Enter your WiFi name
const char *password = "modalcokla";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "kelasiotesp/hasbi/astuti";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

HX711 scale;

WiFiClient espClient;
PubSubClient client(espClient);
uint32_t counter;
char str[80];

void callback(char *topic, byte *payload, unsigned int length) {
 Serial.print("Message arrived in topic: ");
 Serial.println(topic);
 Serial.print("Message:");
 for (int i = 0; i < length; i++) {
     Serial.print((char) payload[i]);
 }
 Serial.println();
 Serial.println("-----------------------");
}

void setup() {
 // Set software serial baud to 115200;
 Serial.begin(115200);
 // connecting to a WiFi network
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.println("Connecting to WiFi..");
 }
 Serial.println("Connected to the WiFi network");
 //connecting to a mqtt broker
 client.setServer(mqtt_broker, mqtt_port);
 client.setCallback(callback);
 while (!client.connected()) {
     String client_id = "esp32-client-";
     client_id += String(WiFi.macAddress());
     Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
     if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
         Serial.println("Public emqx mqtt broker connected");
     } else {
         Serial.print("failed with state ");
         Serial.print(client.state());
         delay(2000);
     }
 }
 // publish and subscribe
 client.publish(topic, "Hi EMQX I'm esp ^^");
 client.subscribe(topic);
}



void loop() {
  client.loop();

  // Read data from the HX711 sensor
  long reading = scale.get_units(10); // Adjust this value for calibration

  // Convert reading to a string
  dtostrf(reading, 6, 2, str);

  // Publish sensor data to MQTT
  client.publish(topic, str);   
 delay(2000);
}

