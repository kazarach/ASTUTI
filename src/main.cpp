#include <ESP8266WiFi.h>
#include <espnow.h>
#include <TaskScheduler.h>

Scheduler taskScheduler;

const int trigPin = D1;  // D1 (GPIO5) on ESP8266
const int echoPin = D2;  // D2 (GPIO4) on ESP8266

// REPLACE WITH RECEIVER MAC Address
uint8_t broadcastAddress[] = {0x24, 0xD7, 0xEB, 0xC9, 0x25, 0x77};

struct struct_message {
  float distance_cm;
  bool servo_moved;

} sensorData;

unsigned long lastTime = 0;
unsigned long timerDelay = 2000;  // send readings timer

void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0) {
    Serial.println("Delivery success");
  } else {
    Serial.println("Delivery fail");
  }
}

void readSensor(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  unsigned long duration = pulseIn(echoPin, HIGH);
  sensorData.servo_moved = false;

  sensorData.distance_cm = duration / 58.2;

  esp_now_send(broadcastAddress, (uint8_t *)&sensorData, sizeof(sensorData));

  // Menampilkan informasi ke Serial Monitor
  Serial.print("Distance: ");
  Serial.print(sensorData.distance_cm);
  Serial.println(" cm");
}

Task taskSensor(5000, TASK_FOREVER, &readSensor);

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  taskScheduler.init();
  taskScheduler.addTask(taskSensor); // Menambahkan objek Task ke scheduler
  taskSensor.enable();
  taskScheduler.startNow(); 
  readSensor(); // Membaca sensor pertama kali saat inisialisasi
}

void loop() {
  taskScheduler.execute();
  // taskSensor.execute();
  // readSensor();
}
