# IoT RFID Attendance System

An embedded IoT system designed to automate attendance tracking using RFID technology and cloud integration.

## Architecture
* **Microcontroller:** ESP8266 (NodeMCU)
* **Sensor:** MFRC522 RFID Reader (SPI Interface)
* **Cloud Integration:** Google Sheets API (via Google Apps Script)
* **Hardware Schematic:** Available in `.fzz` (Fritzing) format.

## How it Works
1. The MFRC522 reads the UID of the RFID card.
2. The ESP8266 processes the UID and sends an HTTP GET request.
3. A Google Apps Script Web App receives the request and logs the timestamp and UID into a Google Sheet in real-time.

## Project Documentation
* A detailed project report is included (`IoT_project_report.docx`).
* Hardware wiring connections are documented in `iot_attendance_connections.png`.
