#include <SPI.h>
#include <MFRC522.h>
#include "pitches.h"

#define RST_PIN         9
#define SS_PIN          10
#define TONE_PIN        5
#define LOOP_MS         10
#define CICLE_MS        10000

MFRC522 mfrc522(SS_PIN, RST_PIN);

//Entrada: E4 3E 2B B9
byte entrada[] = {228, 62, 43, 185};
// Saida: 12 6F 73 65
byte saida[] = {18, 111, 115, 101};

void glissando() {
  tone(TONE_PIN, NOTE_C5, 50);
  delay(50);
  tone(TONE_PIN, NOTE_D5, 50);
  delay(50);
  tone(TONE_PIN, NOTE_E5, 50);
  delay(50);
  tone(TONE_PIN, NOTE_F5, 50);
  delay(50);
  tone(TONE_PIN, NOTE_G5, 50);
  delay(50);
  tone(TONE_PIN, NOTE_A6, 50);
  delay(50);
  tone(TONE_PIN, NOTE_B6, 50);
}

void setup() {
  Serial.begin(9600);
  while (!Serial);
  SPI.begin();
  mfrc522.PCD_Init();
  mfrc522.PCD_DumpVersionToSerial();
  glissando();
}

boolean uidEquals(byte uid1[], byte size1, byte uid2[], byte size2) {
  if(size1 != size2) {
    return false;
  }
  int i;
  for(i = 0; i < size1; i++) {
    if(uid1[i] != uid2[i]) {
      return false;
    }
  }
  return true;  
}
boolean isEntrada(byte uid[], byte size) {
  return uidEquals(uid, size, entrada, 4);
}
boolean isSaida(byte uid[], byte size) {
  return uidEquals(uid, size, saida, 4);
}

boolean temTag() {
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return false;
  }

  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return false;
  }
  return true;
}

void toggle(boolean *flag) {
  if((*flag) == true) {
    *flag = false;
    glissando();
  } else {
    *flag = true;
  }
}

void biparEntrada() {
  tone(TONE_PIN, NOTE_A4, 300);
}
void biparEntrada(int tempo) {
  if(tempo % 500 == 0) {
    biparEntrada();
  }
}

void biparSaida() {
  tone(TONE_PIN, NOTE_A5, 200);
}
void biparSaida(int tempo) {
  if(tempo % 300 == 0) {
    biparSaida();
  }
}

void biparErro() {
  tone(TONE_PIN, NOTE_A5, 200);
  delay(200);
  tone(TONE_PIN, NOTE_A4, 200);
  delay(200);
  tone(TONE_PIN, NOTE_A5, 200);
}

boolean entrando = false;
boolean saindo = false;

void sendUid(byte *uidByte, byte size) {
  if(entrando) {
    Serial.print(F("ENTRADA:"));
  } else if(saindo) {
    Serial.print(F("SAIDA:"));
  } else {
    biparErro();
    return;
  }
  tone(TONE_PIN, NOTE_A6, 100);
  for (byte i = 0; i < size; i++) {
    Serial.print(uidByte[i]);
    Serial.print(" ");
  } 
  Serial.println();
}

int tempo = 0;
void loop() {
  delay(LOOP_MS);
  tempo += LOOP_MS;
  tempo %= CICLE_MS;


  if(tempo % 1000 == 0) {
    Serial.print("Entrando:");
    Serial.println(entrando);
    Serial.print("Saindo:");
    Serial.println(saindo);
  }


  if(entrando) {
    biparEntrada(tempo);
  }

  if(saindo) {
    biparSaida(tempo);
  }

  if(!temTag()) {
    return;
  }

  if(isEntrada(mfrc522.uid.uidByte, mfrc522.uid.size)) {
    if(saindo) {
      biparErro();
      return;
    }
    biparEntrada();
    toggle(&entrando);
    delay(1000);
    return;
  }
  
  if(isSaida(mfrc522.uid.uidByte, mfrc522.uid.size)) {
    if(entrando) {
      biparErro();
      return;
    }
    biparSaida();
    toggle(&saindo);
    delay(1000);
    return;
  }
  
  sendUid(mfrc522.uid.uidByte, mfrc522.uid.size);
  
  if(saindo) {
    saindo = false;
  }
  delay(1000);
}
