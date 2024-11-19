 /*
//#include <Arduino.h>
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);
String tagId = "None";
byte nuidPICC[4];
 
void setup(void) 
{
  Wire.begin();
  Wire.setClock(400000); // Set I2C clock to 400kHz
  Serial.begin(115200);
  Serial.println("System initialized");
  nfc.begin();
 

}
 
void loop() 
{
 //readNFC();

 if (nfc.tagPresent())
 {
  int data_time =micros();
   NfcTag tag = nfc.read();
   Serial.println(micros()-data_time);
   //tag.print();
   tagId = tag.getUidString();
   //Serial.println(micros()-data_time);
 }
  delay(10);
}

void readNFC() 
{
 if (nfc.tagPresent())
 {
   NfcTag tag = nfc.read();
   tag.print();
   //Serial.println(tag);
   tagId = tag.getUidString();
  // Serial.println(tagId);
 }
 delay(500);
}

*/
/*
#include <Wire.h>

#define PN532_I2C_ADDRESS 0x24 // Default I2C address for PN532

void setup() {
  Serial.begin(115200);
  Wire.begin(); // Initialize I2C bus
  Serial.println("PN532 I2C Communication Test");

 
}

void loop() {
  // Nothing in loop
   getFirmwareVersion();
}

void getFirmwareVersion() {
  Wire.beginTransmission(PN532_I2C_ADDRESS);
  Wire.write(0x00); // Pre-amble
  Wire.write(0x00); // Start code 1
  Wire.write(0xFF); // Start code 2
  Wire.write(0x02); // Length
  Wire.write(~0x02 + 1); // Length checksum
  Wire.write(0xD4); // TFI
  Wire.write(0x02); // Command: GetFirmwareVersion
  Wire.write(~(0xD4 + 0x02) + 1); // Checksum
  Wire.write(0x00); // Post-amble
  Wire.endTransmission();

  delay(100); // Wait for response

  Wire.requestFrom(PN532_I2C_ADDRESS, 12); // Request 12 bytes from PN532

  while (Wire.available()) {
    byte c = Wire.read();
    Serial.print(c, HEX);
    Serial.print(" ");
  }
  Serial.println();
}
*/

#include <Wire.h>

#define PN532_I2C_ADDRESS 0x24 // Default I2C address for PN532

void setup() {
  Serial.begin(115200);
  Wire.begin(); // Initialize I2C bus
  Serial.println("PN532 I2C Communication Test");

  // Wake up the PN532
  wakeUpPN532();
  
  // Initialize the PN532 for communication
  initializePN532();
}

void loop() {
  // Continuously check for NFC cards
  if (detectCard()) {
    readCardUID();
  }
  delay(1000); // Delay between checks
}

void wakeUpPN532() {
  Wire.beginTransmission(PN532_I2C_ADDRESS);
  Wire.write(0x00); // Preamble
  Wire.write(0x00); // Start code 1
  Wire.write(0xFF); // Start code 2
  Wire.write(0x02); // Length
  Wire.write(0xFE); // Length checksum
  Wire.write(0xD4); // TFI
  Wire.write(0x14); // Command: RFConfiguration
  Wire.write(0x01); // RF Field: Enabled
  Wire.write(~(0xD4 + 0x14 + 0x01) + 1); // Data Checksum
  Wire.write(0x00); // Postamble
  Wire.endTransmission();

  delay(100); // Wait for response
}

void initializePN532() {
  Wire.beginTransmission(PN532_I2C_ADDRESS);
  Wire.write(0x00); // Preamble
  Wire.write(0x00); // Start code 1
  Wire.write(0xFF); // Start code 2
  Wire.write(0x04); // Length
  Wire.write(0xFC); // Length checksum
  Wire.write(0xD4); // TFI
  Wire.write(0x4A); // Command: InListPassiveTarget
  Wire.write(0x01); // Max target
  Wire.write(0x00); // Baud rate
  Wire.write(~(0xD4 + 0x4A + 0x01 + 0x00) + 1); // Data Checksum
  Wire.write(0x00); // Postamble
  Wire.endTransmission();

  delay(100); // Wait for response
}

bool detectCard() {
  Wire.requestFrom(PN532_I2C_ADDRESS, 20); // Request 20 bytes from PN532
  uint8_t response[20];
  for (int i = 0; i < 20; i++) {
    if (Wire.available()) {
      response[i] = Wire.read();
    }
  }

  // Check for valid response header and length
  if (response[0] == 0x00 && response[1] == 0x00 && response[2] == 0xFF) {
    int len = response[3];
    if (response[4] == (~len + 1)) { // Check length checksum
      if (response[5] == 0xD5 && response[6] == 0x4B) { // Check response command
        return true;
      }
    }
  }
  return false;
}

void readCardUID() {
  Wire.requestFrom(PN532_I2C_ADDRESS, 20); // Request 20 bytes from PN532
  uint8_t response[20];
  for (int i = 0; i < 20; i++) {
    if (Wire.available()) {
      response[i] = Wire.read();
    }
  }

  // Check for valid response header and length
  if (response[0] == 0x00 && response[1] == 0x00 && response[2] == 0xFF) {
    int len = response[3];
    if (response[4] == (~len + 1)) { // Check length checksum
      if (response[5] == 0xD5 && response[6] == 0x4B) { // Check response command
        int uidLen = response[12]; // Length of UID
        Serial.print("UID: ");
        for (int i = 0; i < uidLen; i++) 
        {
          Serial.print(response[13 + i], HEX);
          Serial.print(" ");
        }
        Serial.println();
      }
    }
  }
}

