#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C address might be 0x3F for some LCDs

// List of student UIDs
byte studentUIDs[3][4] = {
  {0x63, 0x57, 0x49, 0x22}, // Sayantan
  {0x93, 0x13, 0x9, 0x2A}, // Soham
  {0x73, 0xB9, 0x4C, 0x22}  // Ayan
};

const char* names[] = {"Sayantan", "Soham", "Ayan"};
const int rolls[] = {42, 30, 10};
bool marked[3] = {false, false, false};  // To avoid marking attendance twice

int totalStudents = 3;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Scan your card");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

  byte* uid = rfid.uid.uidByte;

  int matchedIndex = matchUID(uid);
  if (matchedIndex != -1) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(names[matchedIndex]);
    lcd.setCursor(0, 1);
    lcd.print("Roll: ");
    lcd.print(rolls[matchedIndex]);

    if (!marked[matchedIndex]) {
      Serial.print("Present: ");
      Serial.print(names[matchedIndex]);
      Serial.print(" (Roll ");
      Serial.print(rolls[matchedIndex]);
      Serial.println(")");
      marked[matchedIndex] = true;
    } else {
      Serial.print("Already marked: ");
      Serial.println(names[matchedIndex]);
    }

  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Denied");
    Serial.println("Unknown card scanned");
  }

  delay(2500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scan your card");

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

int matchUID(byte* uid) {
  for (int i = 0; i < totalStudents; i++) {
    bool match = true;
    for (int j = 0; j < 4; j++) {
      if (uid[j] != studentUIDs[i][j]) {
        match = false;
        break;
      }
    }
    if (match) return i;
  }
  return -1;
}

