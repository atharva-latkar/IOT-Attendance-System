#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN  D4   // SDA (GPIO2)
#define RST_PIN D3   // RST (GPIO0)

MFRC522 mfrc522(SS_PIN, RST_PIN);

// debounce to avoid duplicate prints for one tap
String lastUID = "";
unsigned long lastUIDTime = 0;
const unsigned long uidDebounceMs = 1200; // ignore same UID within 1.2s

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println("MFRC522 UID Reader (ESP8266) - place key/tag near reader");

  SPI.begin();            // Init SPI bus (SCK=D5, MISO=D6, MOSI=D7)
  mfrc522.PCD_Init();     // Init MFRC522
  delay(50);

  byte version = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.print("Reader Version (reg 0x37) = 0x");
  Serial.println(version, HEX);
  if (version == 0x91 || version == 0x92) {
    Serial.println("MFRC522 detected ✅");
  } else {
    Serial.println("Warning: unexpected version (may still work).");
  }
}

void loop() {
  // If no new card present, exit quickly
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  // build UID string in uppercase hex without spaces (example: 5C150860)
  String uidCompact = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) uidCompact += "0";
    uidCompact += String(mfrc522.uid.uidByte[i], HEX);
  }
  uidCompact.toUpperCase();

  unsigned long now = millis();
  // debounce same UID
  if (uidCompact == lastUID && (now - lastUIDTime) < uidDebounceMs) {
    mfrc522.PICC_HaltA();
    delay(50);
    return;
  }

  // Print compact UID
  Serial.print("UID (compact) : ");
  Serial.println(uidCompact);

  // Print spaced bytes (e.g. 5C 15 08 60)
  Serial.print("UID (bytes)   : ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) Serial.print("0");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    if (i < mfrc522.uid.size - 1) Serial.print(" ");
  }
  Serial.println();

  // Update debounce trackers
  lastUID = uidCompact;
  lastUIDTime = now;

  // Halt card (allow next detection when removed/re-tapped)
  mfrc522.PICC_HaltA();
  delay(200);
}
