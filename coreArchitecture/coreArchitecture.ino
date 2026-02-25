/*
   ESP8266 Solar Monitoring – Core Architecture Sample
   Demonstrates structured scheduling, communication handling,
   and non-blocking embedded design.
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);

// ================= Timing Control =================
unsigned long lastCoreRun = 0;
unsigned long lastCommCheck = 0;

const unsigned long coreInterval = 100;       // Main logic interval
const unsigned long commInterval = 500;       // Communication interval

// ================= System State =================
float solarVoltage = 0.0;
float batteryVoltage = 0.0;
bool systemHealthy = true;

// ================= Core Functions =================

void readPowerInputs() {
  // Simulated sensor read
  solarVoltage = analogRead(A0) * (3.3 / 1023.0);
  batteryVoltage = solarVoltage - 0.2;
}

void updateLogic() {
  if (batteryVoltage < 11.5) {
    Serial.println("Battery low protection triggered");
  }
}

void handleSlaveCommunication() {
  // Placeholder for UDP / slave device handling
  Serial.println("Checking slave status...");
}

void handleRoot() {
  String json = "{";
  json += "\"solar\":" + String(solarVoltage, 2) + ",";
  json += "\"battery\":" + String(batteryVoltage, 2) + ",";
  json += "\"status\":\"" + String(systemHealthy ? "OK" : "ERROR") + "\"";
  json += "}";

  server.send(200, "application/json", json);
}

void setupWebServer() {
  server.on("/", handleRoot);
  server.begin();
}

// ================= Setup =================

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  setupWebServer();

  Serial.println("System initialized");
}

// ================= Main Loop =================

void loop() {

  unsigned long now = millis();

  // Core logic scheduler (non-blocking)
  if (now - lastCoreRun >= coreInterval) {
    lastCoreRun = now;

    readPowerInputs();
    updateLogic();
  }

  // Communication scheduler
  if (now - lastCommCheck >= commInterval) {
    lastCommCheck = now;

    handleSlaveCommunication();
  }

  // Web server handling (non-blocking)
  server.handleClient();
}