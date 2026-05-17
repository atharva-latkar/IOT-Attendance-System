#include <SPI.h>
#include <MFRC522.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecureBearSSL.h>

#define RST_PIN  D3
#define SS_PIN   D4
#define BUZZER   D2

MFRC522 mfrc522(SS_PIN, RST_PIN);  
MFRC522::MIFARE_Key key;  
ESP8266WiFiMulti WiFiMulti;
MFRC522::StatusCode status;

int blockNum = 2;  
byte bufferLen = 18;
byte readBlockData[18];

String data2;
const String data1 = "https://script.google.com/macros/s/AKfycbzyZMO34O2hk2UK28iFUxQOvS4qnIyVYa_GkGiF2F25K8avDh58h7HizEVlDLCIPFLo/exec?name=";

void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.println("[SETUP] Starting...");

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("AtharvaRedmi", "12345678");

  pinMode(BUZZER, OUTPUT);
  SPI.begin();
  mfrc522.PCD_Init();

  Serial.println("Ready! Scan your card...");
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  Serial.println();
  Serial.println("Card Detected.");

  // Read UID of card
  String uidString = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) uidString += "0";
    uidString += String(mfrc522.uid.uidByte[i], HEX);
  }
  uidString.toUpperCase();

  // 🟢 Map UIDs to names
  if (uidString == "5C150860") uidString = "Atharva Latkar";
  if (uidString == "C3C15A31") uidString = "Bhushan Kadu";
  if (uidString == "8400BB03") uidString = "Prajwal Ingole"; // add your friend's UID

  Serial.print("UID: ");
  Serial.println(uidString);
  Serial.println("Sending data to Google Sheet...");

  // 🧩 Encode name properly
  String encodedName = uidString;
  encodedName.trim();
  encodedName.replace(" ", "%20");

  data2 = data1 + encodedName;
  Serial.println(data2);

  // 🔔 Buzzer feedback
  digitalWrite(BUZZER, HIGH); delay(200);
  digitalWrite(BUZZER, LOW);  delay(200);
  digitalWrite(BUZZER, HIGH); delay(200);
  digitalWrite(BUZZER, LOW);

  if (WiFiMulti.run() == WL_CONNECTED) {
    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    client->setInsecure();

    HTTPClient https;
    Serial.println("[HTTPS] begin...");
    if (https.begin(*client, data2)) {
      Serial.println("[HTTPS] GET...");
      int httpCode = https.GET();

      if (httpCode > 0) {
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }
      https.end();
    } else {
      Serial.println("[HTTPS] Unable to connect");
    }
  }

  mfrc522.PICC_HaltA(); // stop reading until next tap
  delay(1500);
}
