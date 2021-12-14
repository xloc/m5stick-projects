#include <M5StickCPlus.h>
#include <Wire.h>
#include "MFRC522_I2C.h"

void ShowReaderDetails();
void clear_screen();
// 0x28 is i2c address on SDA. Check your address with i2cscanner if not match.
MFRC522 mfrc522(0x28); //Create MFRC522 instance.

auto &lcd = M5.Lcd;
void setup()
{
  M5.begin();
  lcd.setRotation(3);
  lcd.fillScreen(BLACK);
  lcd.setTextPadding(5);
  lcd.setCursor(5, 2);
  lcd.println("M5StackFire MFRC522 v2");
  Wire.begin();       // Initialize I2C
  mfrc522.PCD_Init(); // Init MFRC522

  ShowReaderDetails(); // Show details of PCD - MFRC522 Card Reader details
  // Serial.println(F("Scan PICC to see UID, type, and data blocks..."));
  // lcd.println("Scan PICC to see UID, type, and data blocks...");

  attachInterrupt(37, clear_screen, RISING);
}

void IRAM_ATTR clear_screen()
{
  lcd.fillRect(0, 40, lcd.width(), lcd.height() - 40, TFT_BLACK);
}

void loop()
{
  // Look for new cards, and select one if present
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())
  {
    delay(50);
    return;
  }

  // Now a card is selected. The UID and SAK is in mfrc522.uid.
  auto *uid = &mfrc522.uid;

  lcd.fillRect(0, 40, lcd.width(), lcd.height() - 40, TFT_BLACK);
  lcd.setTextColor(TFT_GREENYELLOW);
  lcd.setCursor(5, 40);

  // UID
  char buffer[50] = {0};
  for (byte i = 0; i < uid->size; i++)
  {
    sprintf(buffer + 3 * i, "%02X ", uid->uidByte[i]);
  }
  Serial.print(F("Card UID:"));
  Serial.println(buffer);
  lcd.println(buffer);

  // PICC type
  byte piccType = mfrc522.PICC_GetType(uid->sak);
  Serial.print(F("PICC type: "));
  Serial.println(mfrc522.PICC_GetTypeName(piccType));
  lcd.setTextColor(TFT_BLUE);
  lcd.setCursor(5, 60);
  lcd.println(mfrc522.PICC_GetTypeName(piccType));

  mfrc522.PICC_HaltA();
  // mfrc522.PICC_DumpToSerial(&mfrc522.uid);
}

void ShowReaderDetails()
{
  // Get the MFRC522 software version
  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.print(F("MFRC522 Software Version: 0x"));
  Serial.print(v, HEX);
  if (v == 0x91)
    Serial.print(F(" = v1.0"));
  else if (v == 0x92)
    Serial.print(F(" = v2.0"));
  else
    Serial.print(F(" (unknown)"));
  Serial.println("");
  // When 0x00 or 0xFF is returned, communication probably failed
  if ((v == 0x00) || (v == 0xFF))
  {
    Serial.println(F("WARNING: Communication failure, is the MFRC522 properly connected?"));
  }
}
