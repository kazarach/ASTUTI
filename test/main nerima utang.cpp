#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

uint8_t receiverMACAddress[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
uint8_t senderMACAddress[] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};//RECEIVER
uint8_t senderMACAddress[] = {0x24, 0xD7, 0xEB, 0xC9, 0x25, 0x77};//RECEIVER

const int trigPin = D1;  // D1 (GPIO5) on ESP8266
const int echoPin = D2;  // D2 (GPIO4) on ESP8266

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  // Trigger the HC-SR04 to send ultrasonic pulses
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure the duration of the echo pulse
  unsigned long duration = pulseIn(echoPin, HIGH);

  // Convert the duration to distance (in centimeters or inches)
  float distance_cm = duration / 58.2;  // Conversion for distance in centimeters
  float distance_in = duration / 148.0; // Conversion for distance in inches

  Serial.print("Distance: ");
  Serial.print(distance_cm);
  Serial.print(" cm   ");
  Serial.print(distance_in);
  Serial.println(" inches");

  delay(1000);  // Wait for a moment before taking the next measurement
}