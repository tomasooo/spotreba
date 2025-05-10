#include <ModbusMaster.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// RS485 připojení
#define RX_PIN D2           
#define TX_PIN D1           
#define RE_DE_PIN D0       

SoftwareSerial rs485(RX_PIN, TX_PIN); 
ModbusMaster node;


const char* WIFI_SSID = "TvojeSSID";
const char* WIFI_PASSWORD = "TvojeHeslo";

// API endpoint (PHP skript na serveru)
const char* serverUrl = "http://10.0.1.107/prijem.php";

void preTransmission()  { digitalWrite(RE_DE_PIN, HIGH); }
void postTransmission() { digitalWrite(RE_DE_PIN, LOW); }

void setup() {
  Serial.begin(115200);
  rs485.begin(9600);
  pinMode(RE_DE_PIN, OUTPUT);
  digitalWrite(RE_DE_PIN, LOW);

  // Inicializace Modbus
  node.begin(1, rs485);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  // Připojení k Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Připojuji k WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi připojeno!");
}

void loop() {
  float napeti = readRegister(0x0000);   
  float proud = readRegister(0x0006);    
  float vykon = readRegister(0x000C);    
  float spotreba = readRegister(0x0156); 

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    String json = "{";
    json += "\"napeti\":" + String(napeti, 2) + ",";
    json += "\"proud\":" + String(proud, 2) + ",";
    json += "\"vykon\":" + String(vykon, 2) + ",";
    json += "\"spotreba\":" + String(spotreba, 2);
    json += "}";

    int httpResponseCode = http.POST(json);
    Serial.print("Odpověď serveru: ");
    Serial.println(httpResponseCode);
    http.end();
  }

  delay(60000); 
}

float readRegister(uint16_t reg) {
  uint8_t result = node.readInputRegisters(reg, 2);
  if (result == node.ku8MBSuccess) {
    uint32_t val = ((uint32_t)node.getResponseBuffer(0) << 16) | node.getResponseBuffer(1);
    float f;
    memcpy(&f, &val, sizeof(f));
    return f;
  } else {
    Serial.print("Chyba čtení registru 0x");
    Serial.println(reg, HEX);
    return 0.0;
  }
}
