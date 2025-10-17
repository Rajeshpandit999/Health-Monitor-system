#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"

MAX30105 particleSensor;

#define MAX_BRIGHTNESS 255

uint32_t irBuffer[100]; // infrared LED sensor data
uint32_t redBuffer[100];  // red LED sensor data

int32_t bufferLength; //data length
int32_t spo2; //SPO2 value
int8_t validSPO2; //indicator to show if the SPO2 calculation is valid
int32_t heartRate; //heart rate value
int8_t validHeartRate; //indicator to show if the heart rate calculation is valid

void setup()
{
  Serial.begin(115200);
  Serial.println("Initializing MAX30102...");

  if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD)) {
    Serial.println("MAX30102 not found. Check wiring/power.");
    while (1);
  }

  byte ledBrightness = 60; // 0=Off to 255=50mA
  byte sampleAverage = 4;
  byte ledMode = 2; // 1 = Red only, 2 = Red + IR
  byte sampleRate = 100;
  int pulseWidth = 411;
  int adcRange = 4096;

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);

  bufferLength = 100;

  Serial.println("Place your index finger on the sensor.");
}

void loop()
{
  for (byte i = 0; i < bufferLength; i++)
  {
    while (particleSensor.available() == false) // Wait for new data
      particleSensor.check();
    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample();
  }

  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer,
                                         &spo2, &validSPO2, &heartRate, &validHeartRate);

  Serial.print("HR=");
  Serial.print(heartRate, DEC);
  Serial.print(", HRvalid=");
  Serial.print(validHeartRate, DEC);
  Serial.print(", SPO2=");
  Serial.print(spo2, DEC);
  Serial.print(", SPO2Valid=");
  Serial.println(validSPO2, DEC);
}
