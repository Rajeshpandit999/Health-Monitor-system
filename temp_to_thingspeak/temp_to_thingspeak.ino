#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// -------- WiFi Credentials --------
const char* ssid = "Datapro - Digital";
const char* password = "Datapro@123$";

// -------- ThingSpeak Settings --------
const char* server = "api.thingspeak.com";
String apiKey = "PEJ0TTYTIKIFWOVN";

// -------- DS18B20 Setup --------
#define ONE_WIRE_BUS D4   // GPIO2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// -------- WiFi Client --------
WiFiClient client;

void setup() {
  Serial.begin(115200);

  // Start DS18B20
  sensors.begin();

  // Connect WiFi
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
}

void loop() {
  // Read temperature
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);

  Serial.print("Body Temperature: ");
  Serial.print(tempC);
  Serial.println(" °C");

  // Send data to ThingSpeak
  if (client.connect(server, 80)) {
    String url = "/update?api_key=" + apiKey + "&field1=" + String(tempC);
    
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + server + "\r\n" +
                 "Connection: close\r\n\r\n");

    Serial.println("Data sent to ThingSpeak");
  } 
  else {
    Serial.println("ThingSpeak connection failed");
  }

  client.stop();

  // ThingSpeak minimum delay = 15 seconds
  delay(15000);
}
