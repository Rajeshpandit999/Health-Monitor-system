#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>

MAX30105 particleSensor;

// WiFi credentials
const char* ssid = "Conference Hall";      // 🔹 your WiFi name
const char* password = "Datapro@123$";     // 🔹 your WiFi password

// ThingSpeak credentials
unsigned long channelID = 3102827;         // 🔹 your channel ID
const char* writeAPIKey = "LKXCO00PGRWB4ZCB"; // 🔹 your write API key

WiFiClient client;

// Data buffers for sensor
#define BUFFER_SIZE 100
uint32_t irBuffer[BUFFER_SIZE];
uint32_t redBuffer[BUFFER_SIZE];

int32_t bufferLength;
int32_t spo2;
int8_t validSPO2;
int32_t heartRate;
int8_t validHeartRate;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nStarting MAX30102 + ThingSpeak...");

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  ThingSpeak.begin(client);

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD)) {
    Serial.println("MAX30102 not found. Please check wiring!");
    while (1);
  }
  Serial.println("MAX30102 found!");

  particleSensor.setup(); // Default setup
  particleSensor.setPulseAmplitudeRed(0x0A);
  particleSensor.setPulseAmplitudeGreen(0);
  particleSensor.setPulseAmplitudeIR(0x0A);

  Serial.println("Place your finger on the sensor.");
}

void loop() {
  bufferLength = BUFFER_SIZE;

  // Read 100 samples for analysis
  for (int i = 0; i < bufferLength; i++) {
    while (particleSensor.available() == false)
      particleSensor.check();
    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample();
  }

  // Run SparkFun algorithm
  maxim_heart_rate_and_oxygen_saturation(
    irBuffer, bufferLength, redBuffer,
    &spo2, &validSPO2, &heartRate, &validHeartRate
  );

  if (validHeartRate && validSPO2) {
    Serial.print("❤️ Heart Rate: ");
    Serial.print(heartRate);
    Serial.print(" bpm | ");
    Serial.print("🩸 SpO2: ");
    Serial.print(spo2);
    Serial.println(" %");

    // Send data to ThingSpeak
    ThingSpeak.setField(1, heartRate);
    ThingSpeak.setField(2, spo2);
    int x = ThingSpeak.writeFields(channelID, writeAPIKey);

    if (x == 200)
      Serial.println("✅ Data sent to ThingSpeak!");
    else
      Serial.println("⚠️ Failed to send data. Check internet.");
  } else {
    Serial.println("❌ Invalid reading, try adjusting finger.");
  }

  delay(15000); // Wait 15 seconds before next upload (ThingSpeak limit)
}
