#include <OneWire.h>
#include <DallasTemperature.h>

// DS18B20 data pin
#define ONE_WIRE_BUS D4  // GPIO2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(115200);
  sensors.begin();
  Serial.println("DS18B20 Temperature Sensor Ready");
}

void loop() {
  sensors.requestTemperatures();   // Send command to get temperature
  float bodyTemp = sensors.getTempCByIndex(0);

  if (bodyTemp == DEVICE_DISCONNECTED_C) {
    Serial.println("Sensor not connected!");
  } else {
    Serial.print("Body Temperature: ");
    Serial.print(bodyTemp);
    Serial.println(" °C");
  }

  delay(2000); // read every 2 seconds
}
