#include <WiFi.h>
#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <fstream>
#include <iostream>
#include <ESP32_FTPClient.h>
#include "esp32_ap.h"
#include "pc_static_ip.h"
#include "ftp_client.h"

// Define the GPIO pins for SPI communication on ESP32
#define SS_PIN 21
#define RST_PIN 22
#define SCK_PIN 18
#define MOSI_PIN 23
#define MISO_PIN 19


MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

// Define UID of the safe card
byte safeCardUID[4] = {0x62, 0x15, 0x48, 0x55};  
//*****************************************************************************************//
void setup() {
  Serial.begin(115200);                                     // Initialize serial communications with the PC
  createESP32AccessPoint();
  configureStaticIPForPC();
  Serial.println("ESP32 is ready. Connect to the Access Point.");
  Serial.println("ESP32 IP: ");
  Serial.println(WiFi.softAPIP());
  initFTPConnection();
  SPI.begin();                                                    // Init SPI bus
  mfrc522.PCD_Init();                                              // Init MFRC522 card
}

void PrintHex(uint8_t *data, uint8_t length) // prints 8-bit data in hex with leading zeroes
{
     char tmp[16];
       for (int i=0; i<length; i++) { 
         sprintf(tmp, "0x%.2X",data[i]); 
         Serial.print(tmp); Serial.print(" ");
       }
}
uint8_t buf[10]= {};
MFRC522::Uid id;
MFRC522::Uid id2;
bool is_card_present = false;


void cpid(MFRC522::Uid *id){
  memset(id, 0, sizeof(MFRC522::Uid));
  memcpy(id->uidByte, mfrc522.uid.uidByte, mfrc522.uid.size);
  id->size = mfrc522.uid.size;
  id->sak = mfrc522.uid.sak;
}

bool cmpid(MFRC522::Uid *id1, MFRC522::Uid *id2){
  return memcmp(id1, id2, sizeof(MFRC522::Uid));
}

void deregister_card(){
  is_card_present = false;
  memset(&id,0, sizeof(id));
}
uint8_t control = 0x00;
void loop() {

  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  MFRC522::StatusCode status;

  //-------------------------------------------

  // Look for new cards
  if ( !mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if ( !mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  
  bool result = true;
  uint8_t buf_len=4;
  cpid(&id);
  Serial.print("NewCard ");
  PrintHex(id.uidByte, id.size);
  Serial.println("");

  // Check if the card is the safe card
  if (memcmp(id.uidByte, safeCardUID, 4) == 0) {
    Serial.println("Safe card");
    writeToFile("log.txt", "Safe");
  } else {
    Serial.println("Not Safe card");
    writeToFile("log.txt", "Unsafe");
  }

  while(true){
    control=0;
    for(int i=0; i<3; i++){
      if(!mfrc522.PICC_IsNewCardPresent()){
        if(mfrc522.PICC_ReadCardSerial()){
          control |= 0x16;
        }
        if(mfrc522.PICC_ReadCardSerial()){
          control |= 0x16;
        }
        control += 0x1;
      }
      control += 0x4;
    }

    if(control == 13 || control == 14){
      //card is still there
    } else {
      break;
    }
  }
  
  Serial.println("CardRemoved");
  writeToFile("log.txt", "Removed");
  delay(500); 

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
