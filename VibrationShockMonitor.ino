// File: VibrationShockMonitor.ino
// Arduino MKR WAN 1300 + Piezo & MPU6050 → LoRaWAN shock events

#include <MKRWAN.h>
#include <Wire.h>
#include "MPU6050.h"

// ===== User LoRaWAN Keys (ABP) =====
static const char DEV_EUI[]  = "YOUR_DEVEUI";
static const char APP_EUI[]  = "YOUR_APPEUI";
static const char APP_KEY[]  = "YOUR_APPKEY";

// ===== Sensor & Threshold Config =====
const int    PIEZO_PIN        = A1;       // Piezo analog input
const int    STATUS_LED_PIN   = 6;        // Join/send status
const int    PIEZO_THRESHOLD  = 600;      // 0–1023
const float  SHOCK_THRESHOLD  = 1.5;      // g-force
const uint32_t SAMPLE_INTERVAL = 200;     // ms

// ===== Globals =====
LoRaModem    modem;
MPU6050      imu;
unsigned long lastSample = 0;

void setup() {
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);
  Serial.begin(115200);
  delay(100);

  // Initialize IMU
  Wire.begin();
  imu.initialize();
  if (!imu.testConnection()) {
    Serial.println("MPU6050 not detected!");
    while (1);
  }

  // Initialize LoRaWAN
  if (!modem.begin(EU868)) {
    Serial.println("Failed to start LoRa modem");
    while (1);
  }
  modem.joinABP(DEV_EUI, APP_EUI, APP_KEY);
  if (!modem.isConnected()) {
    Serial.println("LoRa join failed");
    while (1);
  }
  Serial.println("LoRa joined");
  blinkStatus(2, 100); // two quick blinks
}

void loop() {
  // Sample at defined interval
  if (millis() - lastSample < SAMPLE_INTERVAL) return;
  lastSample = millis();

  // Read piezo
  int piezoVal = analogRead(PIEZO_PIN);
  if (piezoVal < PIEZO_THRESHOLD) return;

  // Read accelerometer
  int16_t ax, ay, az;
  imu.getAcceleration(&ax, &ay, &az);
  float xg = ax / 16384.0;
  float yg = ay / 16384.0;
  float zg = az / 16384.0;
  float accelG = sqrt(xg*xg + yg*yg + zg*zg);

  // Check shock threshold
  if (accelG < SHOCK_THRESHOLD) return;

  // Prepare JSON payload
  String payload = "{";
  payload += "\"piezo\":" + String(piezoVal) + ",";
  payload += "\"accel_g\":" + String(accelG, 2);
  payload += "}";

  // Send packet
  Serial.print("Sending: "); Serial.println(payload);
  modem.beginPacket();
  modem.print(payload);
  int err = modem.endPacket(true);
  if (err > 0) {
    Serial.println("Packet sent");
    blinkStatus(1, 200);
  } else {
    Serial.println("Send failed");
    blinkStatus(3, 100);
  }
}

// Blink the status LED n times
void blinkStatus(int n, int duration) {
  for (int i = 0; i < n; i++) {
    digitalWrite(STATUS_LED_PIN, HIGH);
    delay(duration);
    digitalWrite(STATUS_LED_PIN, LOW);
    delay(duration);
  }
}
