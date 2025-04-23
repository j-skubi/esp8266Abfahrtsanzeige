
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

#define SSID "Obi WLAN Kenobi"
#define WIFI_PSK "U9GOTCSRSM"

#define ISMANING "de:09184:2040"

void wifiSetup();
void getStationData();

void setup() {

  Serial.begin(9600);
  Serial.println();
  Serial.println();
  Serial.println();

  wifiSetup();

}

void loop() {

  JsonArray ismaning = getStationData(ISMANING);
  Serial.println(ismaning[0]["label"].as<String>());

  delay(2000000);
}

JsonArray getStationData(String id) {

  JsonArray empty;

  if (!WiFi.isConnected()) {
    return empty;
  }

  WiFiClientSecure client;
  HTTPClient https;

  Serial.print("[HTTPS] begin...\n");

  client.setInsecure();
  https.begin(client, "https://www.mvg.de/api/bgw-pt/v3/departures?globalId=" + id);

  Serial.print("[HTTPS] GET...");
  Serial.print("https://www.mvg.de/api/bgw-pt/v3/departures?globalId=" + id + "\n");

  int httpCode = https.GET();

  if (!(httpCode == 200)) {
  Serial.printf("[HTTPS] GET failed: %d\n", httpCode);
    return empty;
  }

  const String& payload = https.getString();
  Serial.println("received payload:\n<<");
  Serial.println(payload);
  Serial.println(">>");

  JsonDocument filter;
  filter[0]["plannedDepartureTime"] = true;
  filter[0]["realtimeDepartureTime"] = true;
  filter[0]["label"] = true;
  filter[0]["destination"] = true;

  JsonDocument doc;

  DeserializationError error = deserializeJson(doc, payload, DeserializationOption::Filter(filter));

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return empty;
  }

  return doc.as<JsonArray>();
}

void wifiSetup() {

  WiFi.begin(SSID, WIFI_PSK);

  Serial.println("Connecting!");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

}