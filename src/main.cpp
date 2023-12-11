#include <Arduino.h>
#include "HX711.h"

const int DOUT_PIN = D7;  // Replace with your ESP8266 GPIO pin connected to DT
const int SCK_PIN = D8;   // Replace with your ESP8266 GPIO pin connected to SCK

HX711 scale;

void setup() {
    Serial.begin(9600);
    scale.begin(DOUT_PIN, SCK_PIN); // Initialize HX711 object with pins
    scale.set_scale(1.0);           // Set calibration factor if needed
    scale.tare();                   // Reset the scale to 0

    delay(100); // Adding a small delay after initialization
}

void loop() {
    float weight = scale.get_units(10); // Read weight (change 10 to the average times)
    Serial.print("Weight: ");
    Serial.println(weight);
    delay(1000); // Adjust the delay based on your needs
}
