#include <WiFi.h>
#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ESPUI.h>
#include "esp32_ap.h"

// Define the GPIO pins for SPI communication on ESP32
#define SS_PIN 21
#define RST_PIN 22
#define SCK_PIN 18
#define MOSI_PIN 23
#define MISO_PIN 19

String state = "Unknown";

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;


MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

// Define UID of the safe card
byte safeCardUID[4] = {0x62, 0x15, 0x48, 0x55};  
//*****************************************************************************************//
void setup() {
  Serial.begin(115200);                                     // Initialize serial communications with the PC
  createESP32AccessPoint();
  Serial.println("ESP32 is ready. Connect to the Access Point.");
  Serial.println("ESP32 IP: ");
  Serial.println(WiFi.softAPIP());
  SPI.begin();                                                    // Init SPI bus
  mfrc522.PCD_Init();                                              // Init MFRC522 card
  ESPUI.label("Card Status: ", ControlColor::Turquoise, state); // Start the ESPUI framework
  ESPUI.begin("Sistem Antifurt");
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

//loop
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
    state = "Safe";
    ESPUI.updateLabel(1, state);
  } else {
    Serial.println("Not Safe card");
    state = "Unsafe";
    ESPUI.updateLabel(1, state);
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
  state = "Removed";
  ESPUI.updateLabel(1, state);
  delay(2000); 

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
